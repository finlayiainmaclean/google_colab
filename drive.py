from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
import os, sys
import argparse
import zipfile

GDRIVE_PROJECT_FOLDER = os.environ['PROJECT_NAME']
gauth = GoogleAuth()
# Try to load saved client credentials
gauth.LoadCredentialsFile("mycreds.txt")
if gauth.credentials is None:
    # Authenticate if they're not there
    # This is what solved the issues:
    gauth.GetFlow()
    gauth.flow.params.update({'access_type': 'offline'})
    gauth.flow.params.update({'approval_prompt': 'force'})
    gauth.LocalWebserverAuth()
elif gauth.access_token_expired:
    # Refresh them if expired
    gauth.Refresh()
else:
    # Initialize the saved creds
    gauth.Authorize()
# Save the current credentials to a file
gauth.SaveCredentialsFile("mycreds.txt")  
drive = GoogleDrive(gauth)

def zip(file_to_zip, zipped_file, type='file'):
    print(file_to_zip)
    print(zipped_file)
    assert type in ['file','folder']
    zipf = zipfile.ZipFile(zipped_file, 'w', zipfile.ZIP_DEFLATED)
    path_to_zip = file_to_zip if type=='file' else f"{file_to_zip}/"
    zipdir(path_to_zip, zipf)
    zipf.close()
    return zipped_file

def zipdir(path, ziph):
    # ziph is zipfile handle
    for root, dirs, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file), os.path.relpath(os.path.join(root, file), os.path.join(path, '..')))

def get_folder_id_from_name(folder_name):
    folder_id = ''
    file_list = drive.ListFile({'q': "'root' in parents and trashed=false"}).GetList()
    for file in file_list:
        if(file['title'] == folder_name):
            folder_id = file['id']
            return folder_id
    print(f"Couldn't find folder {folder_name}")
    return None

def get_file_id_from_folder_name_and_file_name(folder_name, file_name):

    folder_id = get_folder_id_from_name(folder_name)
    file_list = drive.ListFile({'q':f"'{folder_id}'  in parents and trashed=False"}).GetList()
    file_id = None
    for x in range(len(file_list)):
        if file_list[x]['title'] == file_name:
            file_id = file_list[x]['id']
            return file_id
    print(f"Couldn't find file {file_name}")

def update_to_gdrive(folder_title, file_name, file_id):
    folder_id = get_folder_id_from_name(folder_title)
    file1 = drive.CreateFile({'id':file_id})
    file1.SetContentFile(file_name)
    file1.Upload()
    print(f"Updated file {file_name}")


def upload_to_gdrive(folder_title, file_name):
    folder_id = get_folder_id_from_name(folder_title)
    file1 = drive.CreateFile({'parents': [{'id': folder_id}]})
    file1.SetContentFile(file_name)
    file1.Upload()
    print(f"Uplodaded file {file_name}")

def download_from_gdrive(folder_title, file_name):
    file_id = get_file_id_from_folder_name_and_file_name(folder_title, file_name)
    file = drive.CreateFile({'id': file_id})
    file.GetContentFile(file_name)
 
if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('-p','--paths', nargs='+', help='<Required> Set flag', required=True)
    parser.add_argument('-u','--upload', dest='upload', action='store_true')
    parser.add_argument('-d','--download', dest='download', action='store_true')
    args = parser.parse_args()


    if args.upload:
        for path in args.paths:
            path_formatted = path.replace('/','__')
            path_formatted_zip = f'{path_formatted}.zip'
            _ = zip(path[:-1], path_formatted_zip , type='folder')

            file_id = get_file_id_from_folder_name_and_file_name(GDRIVE_PROJECT_FOLDER, path_formatted_zip)

            if file_id is None:
                upload_to_gdrive(GDRIVE_PROJECT_FOLDER, path_formatted_zip)
            else:
                update_to_gdrive(GDRIVE_PROJECT_FOLDER, path_formatted_zip, file_id)

            os.remove(path_formatted_zip)

    elif args.download:
        for path in args.paths:
            print('path',path)
            download_from_gdrive(GDRIVE_PROJECT_FOLDER,path)
            reformatted_path = path.replace('__','/')
            reformatted_path = reformatted_path.replace('/.zip','.zip').replace('.zip','')
            one_up_path = '/'.join(reformatted_path.split('/')[:-1])
            with zipfile.ZipFile(path,"r") as zip_ref:
                zip_ref.extractall(one_up_path)
            os.remove(path)
    else:
        raise ValueError('Need upload or download flag')
        

        
        

