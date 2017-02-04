#ifndef _7ZLIB_H
#define _7ZLIB_H

size_t Decompress7zArchive(std::string const& archivePath, std::string rootPath);
size_t Decompress7zArchive(std::string const& archivePath);

#endif // _7ZLIB_H
