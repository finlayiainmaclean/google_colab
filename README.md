google_colab
==============================

*Set up PyDrive (only once for all projects)
1. In Google Drive console add project + web app (localhost:8000)
2. In Credentials add OAuth user
3. Enable Google Drive API
4. Run `drive_auth.py`

*Set up project*
1. Create project folder in Google Drive `root/$project_name`
2. Create git project with same project name
3. Create environment variable of project name `export PROJECT_NAME=google_colab `
4. Change project name in Makefile and run `make create_environment`

*To use GPU/TPU to run script and save results back*
1. LOCAL: Upload data + zip and models from localhost (`python upload.py`)
2. COLAB: Download data and models + unzip to colab (use `notebooks/colab.ipynb` as template)
3. COLAB: Run src script (`src/colab.py` as template)
4: COLAB: Reupload zipped folders to google drive (`notebooks/colab.ipynb` has code section)
5: LOCAL: Download + unzip to local  (`python download.py`)
