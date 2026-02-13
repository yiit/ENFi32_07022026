import gzip

# Extract the gzipped index file
with gzip.open('index.htm.gz', 'rb') as f_in:
    with open('index.html', 'wb') as f_out:
        f_out.write(f_in.read())

print("Successfully extracted index.htm.gz to index.html")