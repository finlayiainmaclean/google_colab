from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
import os, sys
import zipfile

GDRIVE_FOLDER = os.environ['PROJECT_NAME']
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

def download_from_gdrive(folder_title, file_name):

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
        file = drive.CreateFile({'id': file_id})
        file.GetContentFile(file_name)
        print(f'Saved file {file_name}')
    else:
        print(f'Couldnt find file {file_name}')

def download_and_unip(folder):
    download_from_gdrive(GDRIVE_FOLDER,f'{folder}.zip')
    with zipfile.ZipFile(f'{folder}.zip',"r") as zip_ref:
        zip_ref.extractall("")
    os.remove(f'{folder}.zip')


if __name__ == "__main__":
    folders_to_zip = ['data','models']
    for folder in folders_to_zip:
        download_and_unip(folder)

