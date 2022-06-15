import tweepy
import configparser
import pandas as pd

config = configparser.ConfigParser()
config.read('config.ini')

api_key = config['twitter']['api_key']
api_key_secret = config['twitter']['api_key_secret']

access_token = config['twitter']['access_token']
access_token_secret = config['twitter']['access_token_secret']

auth = tweepy.OAuthHandler(api_key, api_key_secret)
auth.set_access_token(access_token, access_token_secret)

api = tweepy.API(auth)

users = ['BBCBreaking', 'cnni', 'Reuters', 'nytimes', 'guardian', 'WSJ']
limit=1000

# for user in users:
#     tweets = tweepy.Cursor(api.user_timeline, screen_name = user, count = 500, tweet_mode = 'extended').items(limit)
#     news = []
#     for tweet in tweets:
#         news.append([tweet.id , tweet.created_at, tweet.full_text])
#     df = pd.DataFrame(news, columns = ['id', 'created_at', 'news'])
#     df.to_csv(user+".csv", index = False)

keywords_list = ['Covid', 'Vaccine', 'S&P500', 'Dow Jones', 'Stock Market', 'Natural Disaster', 'Pollution', 'War', 'Renewable Energy', 'Smart Cities']

for keywords in keywords_list:
    tweets = tweepy.Cursor(api.search_tweets, q = keywords, lang = "en", count = 500, tweet_mode = 'extended').items(limit)
    data = []
    for tweet in tweets:
        # print(tweet.user.name + " ~ " + tweet.user.location + " ~ " + tweet.user.description)
        data.append([tweet.id , tweet.created_at, tweet.user.screen_name, tweet.user.location, tweet.user.description, tweet.full_text])
    df = pd.DataFrame(data, columns = ['id', 'created_at', 'user_screen_name', 'user_location', 'user_description', 'full_text'])
    df.to_csv(keywords.replace(" ", "_")+".csv", index = False)
    print(df.shape)