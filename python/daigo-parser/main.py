from bs4 import BeautifulSoup
from tqdm import tqdm
import urllib.request
import re
import os

LINKS_PATH = "links.txt"
RESULT_HTML_PATH = "result.html"
pages = 277
URL = "https://ch.nicovideo.jp/mentalist/blomaga"
AR_URL = "https://ch.nicovideo.jp"

def fetch_html(url:str):
    req = urllib.request.Request(url)
    with urllib.request.urlopen(req) as resp:
        body = resp.read()
    return body


def parse_body(html, links):
    soup = BeautifulSoup(html, "html.parser")

    # 本文の link だけ取ってくる
    main_blog_div = soup.find_all("div", class_="main_blog_txt")
    for div in main_blog_div:
        for a in div.find_all("a"):
            href = a.get("href")
            links.append(href)
        
    # 記事を読むの href を取得
    ar_links = []
    div_mores = soup.find_all("div", class_="more")
    for div in div_mores:
        link = div.find("a")
        ar_links.append(link["href"])
    return ar_links, links


def filter_links(links):
    links = list(set(links))
    # なぜこんな一文が必要なんだろうか
    links = [s for s in links if s is not None]
    return links


# 短縮URLを復元する
def convert_links(links):
    # 短縮URL をもとに戻す
    #XXX 汚い
    for i in range(len(links)):
        link = links[i]
        if "amzn.to" in link:
            try:
                links[i] = urllib.request.urlopen(link).geturl()
            except urllib.error.HTTPError as e:
                print('---raise HTTPError---')
                print(e.code)
                print(e.reason)
                print(link)
                continue
            except urllib.error.URLError as e:
                print('---rase URLError---')
                print(e.reason)
                print(link)
                continue
    return links


def fetch():
    links = []
    ar_links = []
    for i in tqdm(range(1, pages+1)):
        body = fetch_html(f'{URL}?page={i}')
        ar_link, links = parse_body(body, links)
        ar_links.extend(ar_link)

    for ar_link in tqdm(ar_links):
        body = fetch_html(f'{AR_URL}{ar_link}')
        _, links = parse_body(body, links)

    links = filter_links(links)
    links = convert_links(links)

    return links


def load():
    links = []
    with open(LINKS_PATH, 'r') as f:
        for s in f:
            links.append(s.rstrip('\n'))
    return links


def search(links):
    links = [x for x in links if "amazon" in x or "amzn" in x]
    return links


def generate_html(asin_url):
    html_body = ""
    for asin, link in asin_url:
        img_url = f'http://images-jp.amazon.com/images/P/{asin}.09.MZZZZZZZ'
        b = f'<a href="{link}"><img src="{img_url}"></a>\n'
        html_body += b
    return html_body


def generate_asin_url(links):
    # [ASIN, link]
    asin_url = []
    for link in links:
        # ASIN を抽出
        # 任意の数字またはアルファベットの10回の繰り返し
        pattern = '.*([\w]{10}).*'

        asin = re.match(pattern, link)
        if asin:
            asin = asin.group(1)
            asin_url.append([asin, link])
    return asin_url


def main():
    links = []
    if os.path.isfile(LINKS_PATH):
        links = load()
    else:    
        links = fetch()
        with open(LINKS_PATH, 'w') as f:
            f.write("\n".join(links))

    links = search(links) 
    asin_url = generate_asin_url(links)
    html_body = generate_html(asin_url)
    with open(RESULT_HTML_PATH, "w") as f:
        f.write(html_body)    


if __name__ == "__main__":
    main()