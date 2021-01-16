from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
import os, sys
import zipfile

GDRIVE_FOLDER = 'google_colab'
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


def zipdir(path, ziph):
    # ziph is zipfile handle
    for root, dirs, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file), os.path.relpath(os.path.join(root, file), os.path.join(path, '..')))

def upload_to_gdrive(folder_title, file_name):

    folder_id = ''

    file_list = drive.ListFile({'q': "'root' in parents and trashed=false"}).GetList()
    for file in file_list:
        if(file['title'] == folder_title):
            folder_id = file['id']
            break

    file_list = drive.ListFile({'q':f"'{folder_id}'  in parents and trashed=False"}).GetList()

    file_id = None
    for x in range(len(file_list)):
        if file_list[x]['title'] == file_name:
            file_id = file_list[x]['id']
            break

    if file_id is not None:
        file1 = drive.CreateFile({'id':file_id})
        file1.SetContentFile(file_name)
        file1.Upload()
        print(f"Updated file {file_name}")
    else:
        file1 = drive.CreateFile(metadata={"title": file_name})
        file1.SetContentFile(file_name)
        file1.Upload()
        print(f"Uplodaded file {file_name}")

    del file1

def zip_and_upload(folder):
    zipf = zipfile.ZipFile(f'{folder}.zip', 'w', zipfile.ZIP_DEFLATED)
    zipdir(f'{folder}/', zipf)
    zipf.close()
    upload_to_gdrive(GDRIVE_FOLDER, f'{folder}.zip')
    os.remove(f'{folder}.zip')

if __name__ == "__main__":
    folders_to_zip = ['data','models']
    for folder in folders_to_zip:
        zip_and_upload(folder)

