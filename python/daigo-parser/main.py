from bs4 import BeautifulSoup
from tqdm import tqdm
import urllib.request


LINKS = []
pages = 5
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


def filter(LINKS):
    


def main():
    ar_links = []
    for i in tqdm(range(1, pages+1)):
        body = fetch_html(f'{URL}?page={i}')
        links = parse_body(body)
        ar_links.extend(links)

    for ar_link in tqdm(ar_links):
        body = fetch_html(f'{AR_URL}{ar_link}')
        parse_body(body)

    # body = fetch_html(f'{URL}')
    # ar_links = parse_body(body)
    print("---------------LINKS---------------")
    print(LINKS)
    print("---------------ar_links---------------")
    print(ar_links)

    print("---------------filter---------------")



if __name__ == "__main__":
    main()