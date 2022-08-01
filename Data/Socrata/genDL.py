import json
from urllib.request import urlopen

domain_categories = ['Earth Science', 'Government', 'Health', 'Public Safety', 'Education', 
                     'Transportation', 'Environment', 'Finance', 'Social Services', 'Demographics']

url = 'http://api.us.socrata.com/api/catalog/v1?'
# url = 'http://api.us.socrata.com/api/catalog/v1/domain_categories?'
offset = 0
limit = 100

for i in range(1):
    response = urlopen(url+'offset='+str(offset)+'&limit='+str(limit))
    metadata_set = json.loads(response.read())['results']

    for metadata in metadata_set:
        # print(metadata['domain_category'])
        if( len(metadata['resource']['columns_field_name']) > 0 and len(metadata['classification']['domain_category']) > 0):
            print(metadata['classification']['domain_category'])

