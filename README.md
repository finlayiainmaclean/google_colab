google_colab
==============================

*Set up PyDrive (only once for all projects)*
1. Install pydrive `pip install PyDrive`
2. Go to https://console.developers.google.com/apis/dashboard and create a new project
3. Create web application and set both URLs to `localhost:8000`
4. Under OAuth user under credentials.
5. Under OAuth add your email as a test user
6. Download credentials as json, put in working directory and change name to `client_secrets.json`.
7. Go to Google Drive API and enable
8. Run `drive_auth.py`

*Set up project*
1. Create project folder in Google Drive `root/$project_name`
2. Create git project with same project name
3. Create environment variable of project name `export PROJECT_NAME=$project_name`
4. Change project name in Makefile and run `make create_environment`

*To use GPU/TPU to run script to load `data` and `models` and save results back*
1. LOCAL: Upload data + zip and models from localhost (`python upload.py`)
2. LOCAL: Push local changes to git repo
3. COLAB: Load notebook from git repo (`notebooks/colab.ipynb` as template), clone git repo + set up env
4. COLAB: Download from google drive + unzip to colab (use `notebooks/colab.ipynb` as template) to repo `data` and `models` folders
5. COLAB: `notebooks/colab.ipynb` runs src script (`src/colab.py` as template), using GPU/TPU
6: COLAB: Reupload zipped folders to google drive (`notebooks/colab.ipynb` has code section)
7: LOCAL: Download + unzip to local  (`python download.py`)
