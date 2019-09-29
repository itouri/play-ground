from bs4 import BeautifulSoup
from tqdm import tqdm
import urllib.request
import re
import os

FILE_PATH = "links.txt"
LINKS = []
pages = 277
URL = "https://ch.nicovideo.jp/mentalist/blomaga"
AR_URL = "https://ch.nicovideo.jp"

def fetch_html(url:str):
    req = urllib.request.Request(url)
    with urllib.request.urlopen(req) as resp:
        body = resp.read()
    return body


def parse_body(html):
    soup = BeautifulSoup(html, "html.parser")

    main_blog_div = soup.find_all("div", class_="main_blog_txt")
    for div in main_blog_div:
        for a in div.find_all("a"):
            href = a.get("href")
            LINKS.append(href)
        
    ar_links = []
    div_mores = soup.find_all("div", class_="more")
    for div in div_mores:
        link = div.find("a")
        ar_links.append(link["href"])
    return ar_links


def filter_links():
    global LINKS

    LINKS = list(set(LINKS))
    # なぜこんな一文が必要なんだろうか
    LINKS = [s for s in LINKS if s is not None]


def fetch():
    ar_links = []
    for i in tqdm(range(1, pages+1)):
        body = fetch_html(f'{URL}?page={i}')
        links = parse_body(body)
        ar_links.extend(links)

    for ar_link in tqdm(ar_links):
        body = fetch_html(f'{AR_URL}{ar_link}')
        parse_body(body)

    filter_links()

    with open(FILE_PATH, 'w') as f:
        f.write("\n".join(LINKS))
#        pickle.(LINKS, f)


def load():
    with open(FILE_PATH, 'r') as f:
        global LINKS
        for s in f:
            LINKS.append(s.rstrip('\n'))
        # LINKS = pickle.load(f)


def search():
    global LINKS
    LINKS = [x for x in LINKS if "amazon" in x]


def main():
    if os.path.isfile(FILE_PATH):
        load()
    else:    
        fetch() 

    search()
    
    # ASIN を抽出
    # 任意の数字またはアルファベットの10回の繰り返し
    pattern = '.*([\w]{10}).*'
    asins = []
    asin_url = []
    for link in LINKS:
        asin = re.match(pattern, link)
        asin = asin.group(1)
        # group() の引数って何？
        #print(asin.group(1))
        asins.append(asin)
        asin_url.append([asin, link])

    html_body = ""
    for asin, link in asin_url:
        img_url = f'http://images-jp.amazon.com/images/P/{asin}.09.MZZZZZZZ'
        b = f'<a href="{link}"><img src="{img_url}"></a>\n'
        html_body += b

    with open("result.html", "w") as f:
        f.write(html_body)
    #print( "\n".join( ",".join(asin_url) ) )
    


if __name__ == "__main__":
    main()