import requests
import re
from bs4 import BeautifulSoup

BASE = 'https://en.wikipedia.org'
URL = f'{BASE}/wiki/List_of_galaxies'
OUTDIR = './docs'

if __name__ == '__main__':
    crawl = set()

    page = requests.get(URL)
    soup = BeautifulSoup(page.content, 'html.parser')
    content = soup.find(id='mw-content-text')
    links = content('a')
    for l in links:
        href = l.get('href')
        if not isinstance(href, str):
            continue

        match = re.search('^\/wiki\/.+', href)
        if not match:
            continue

        match = re.search('File|Special', href)
        if match:
            continue

        crawl.add(href)

    doc = 0
    sorted(crawl)
    for l in crawl:
        try:
            url = f'{BASE}{l}'
            page = requests.get(url)
            soup = BeautifulSoup(page.content, 'html.parser')

            title = soup.find(id='firstHeading')
            content = soup.find(id='mw-content-text')
            ps = content.find_all('p')

            if not title.string:
                continue

            filename = title.string.strip()

            text = ''

            i = 1
            for p in ps:
                for string in p.strings:
                    text += repr(string)
                i += 1

            text = text.replace("'", '')
            text = text.replace('"', '')
            text = text.replace('\\n', '\n')
            text = re.sub('\[\d+\]', '', text)

            if not text:
                continue

            text = text.strip()
            if not text:
                continue

            text = url + '\n' + filename + '\n' + text
            text = text.encode('ascii')

            f = open(f'{OUTDIR}/{doc}.txt', "wb")
            f.write(text)
            f.close()
            doc += 1
        except Exception as e:
            continue
