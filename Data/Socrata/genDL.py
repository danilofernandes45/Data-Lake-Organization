import json
from urllib.request import urlopen
import numpy as np
import pandas as pd
import re

# categories = ['Earth Science', 'Government', 'Health', 'Public Safety', 'Education', 
#                      'Transportation', 'Environment', 'Finance', 'Social Services', 'Demographics']

url = 'http://api.us.socrata.com/api/catalog/v1?only=datasets'
# url = 'http://api.us.socrata.com/api/catalog/v1/domain_categories?'
# offset = 0
limit = 100

metaDL = []

for i in range(20):
    offset = i * 100
    response = urlopen(url+'&offset='+str(offset)+'&limit='+str(limit))
    metadata_set = json.loads(response.read())['results']

    for metadata in metadata_set:
        if( len(metadata['resource']['columns_field_name']) > 0 
            and len(metadata['classification']['categories']) > 0 
            and ('Text' in metadata['resource']['columns_datatype']) ):
            metaDL.append(metadata)

print(len(metaDL))

DL = []
i = 0
NUM_COLUMNS = 0
while len(DL) < 1000 and i < len(metaDL):
    metadata = metaDL[i]
    i += 1
    url = "http://" + metadata['metadata']['domain'] + "/resource/" + metadata['resource']['id'] + ".json?"
    url += "$limit=150"

    types = np.array(metadata['resource']['columns_datatype'])
    columns = np.array(metadata['resource']['columns_field_name'])
    columns = columns[np.where(types == 'Text')]

    url += "&$select="+",".join(columns)
    print(url)
    try:
        data = pd.read_json(url)

        # REMOVE COLUMNS WITHOUT TEXT
        data = data.applymap(lambda s : str(s).lower()) #LOWERCASE
        data = data.applymap(lambda s : re.sub(r'[^a-z\s]', '', s)) #REMOVE NO-LETTERS
        data = data.applymap(lambda s : re.sub(r'\s+', ' ', s)) #REMOVE EXTRA BLANK SPACES

        data.replace('', float('NaN'), inplace = True) # '' -> NaN
        data.dropna(how='all', axis=1, inplace=True) # Remove columns only filled with NaN

        print(data.shape[1])
        NUM_COLUMNS += data.shape[1]
        print(NUM_COLUMNS)
    except :
        print('NOT FOUND')
