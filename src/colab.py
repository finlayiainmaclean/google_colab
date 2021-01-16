import rdkit
import os, sys
import pandas as pd

print(os.getcwd())

df = pd.read_csv('data/external/test.csv', header=None)
df = df*2
print(df)
df.to_csv('data/external/test.csv', index=False, header=False)   