/**
 * @file    TextFile.h
 * @author  Phil Lockett <phillockett65@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Template for basic text file read/write handling.
 */

#if !defined(_TEXTFILE_H__20210503_1300__INCLUDED_)
#define _TEXTFILE_H__20210503_1300__INCLUDED_

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>


/**
 * @section text file read/write handling interface.
 *
 */

template<typename T=char>
class TextFile
{
public:
    using Iterator = std::vector<std::basic_string<T>>::const_iterator;

    TextFile(const std::string & file) : fileName{file} {}
    TextFile(const std::filesystem::path & file) : fileName{file} {}
    virtual ~TextFile(void) {}

    TextFile(const TextFile & other) : fileName{other.fileName}, data{other.data} {}
    void operator=(const TextFile & other) { fileName = other.fileName; data = other.data; }

    friend std::ostream & operator<<(std::ostream &os, const TextFile &A) { A.display(os); return os; }

    void setData(const std::vector<std::basic_string<T>> & other) { data = other; }
    std::vector<std::basic_string<T>> getData() { return data; }
    const std::vector<std::basic_string<T>> getData() const { return data; }
    std::vector<std::basic_string<T>> moveData() noexcept { return std::move(data); }
    void moveData(std::vector<std::basic_string<T>> && other) noexcept { data = std::move(other); }

    bool equal(const TextFile & other) const;
    bool equal(const TextFile & other, size_t count) const { return std::equal(data.begin(), data.begin()+count, other.data.begin()); }
    void clear(void) { data.clear(); }

    void setFileName(const std::string & file) { fileName = file; }
    void setFileName(const std::filesystem::path & file) { fileName = file; }
    std::string getFileName(void) const { return fileName.c_str(); }
    bool exists(void) const { return std::filesystem::exists(fileName); }

    void reserve(size_t size) { data.reserve(size); }
    size_t size(void) { return data.size(); }
    Iterator begin(void) { return data.begin(); }
    Iterator end(void) { return data.end(); }

    int write(const std::vector<std::basic_string<T>> & other) { setData(other); return write(); }
    int write(void) const;
    int read(int reserve = 100);

private:
    std::filesystem::path fileName;
    std::vector<std::basic_string<T>> data;

};


/**
 * @section text file read/write handling implementation.
 *
 */

/**
 * @brief Compares the data of the supplied TextFile equals this data.
 * 
 * @tparam T Char type.
 * @param other the suplied TextFile to compare.
 * @return true if the data of the suplied TextFile equals this data.
 * @return false otherwise.
 */
template<typename T>
bool TextFile<T>::equal(const TextFile & other) const
{
    if (data.size() != other.data.size())
        return false;

    return std::equal(data.begin(), data.end(), other.data.begin());
}


/**
 * @brief Write the buffer to the named file.
 * 
 * @tparam T Char type.
 * @return int error value or 0 if no errors.
 */
template<typename T>
int TextFile<T>::write(void) const
{
    if (std::basic_ofstream<T> os{fileName, std::ios::out})
    {
        for (const auto & line : data)
            os << line << '\n';

        return 0;
    }

    return 1;
}


/**
 * @brief Read the named file into the buffer.
 * 
 * @tparam T Char type.
 * @param res reserve the number of lines in the buffer.
 * @return int error value or 0 if no errors.
 */
template<typename T>
int TextFile<T>::read(int res)
{
    const std::basic_string<T> tokens{T('\r'), T('\n'), T('\0')};
    if (std::basic_ifstream<T> is{fileName, std::ios::in})
    {
        reserve(res);
        std::basic_string<T> line;

        while (getline(is, line))
        {
            const auto pos{line.find_first_of(tokens)};
            if (pos != std::basic_string<T>::npos)
                line = line.substr(0, pos);
            if (!is.eof() && line.length())
                data.push_back(std::move(line));
        }

        return 0;
    }

    return 1;
}


#endif // !defined(_TEXTFILE_H__20210503_1300__INCLUDED_)

