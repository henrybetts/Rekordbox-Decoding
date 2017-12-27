# Introduction
This document attempts to describe the format of the Pioneer .pdb database file, which can be found on removable storage devices that have been synced with RecordBox. The .edb file, found on the host RecordBox system has a different format, though the database schema seems to be the same.

The .pdb file is a relational database. It’s data is organised into tables of rows and columns. For example, a typical .pdb file will have have a table containing rows that represent tracks, and another table that contains rows representing artists. Each row in a table has a unique id, and the data for each row is stored in the same layout, according to the table schema.


# File Structure

The file consists of fixed size blocks of 4096 bytes. I have not yet managed to decode the data in the first block, although the 4 byte integer at 0x04 is always 4096, so this probably defines the block size. Integers are stored in little endian format.

The remaining blocks each consist of a 40 byte header, followed by some row data. The rows within a block all seem to belong to the same table, though a block might not contain all of the rows for a table, since the block has a fixed size. Therefore, large tables will be split across multiple blocks.

Here’s what I’ve decoded from the 40 byte header so far:

| Location    | Data type | Description             |
| ----------- | --------- | ----------------------- |
| 0x00 - 0x03 |           | Zero?                   |
| 0x04        | uint32    | Block id / index        |
| 0x08 - 0x17 |           | Unknown                 |
| 0x18        | uint8     | Number of rows in block |
| 0x19 - 0x27 |           | Unknown                 |

The bytes at 0x20 and 0x22 tend to add up to the number of rows in the block.


# Row Structure

Each row can be broken into three sections; a four byte header, followed by the fixed size column data, followed by variable sized string data.

The first two bytes of the header indicate the table that the row belongs to. So each table has a unique two byte id. The next two bytes seem to represent an index, or an id for rows within just that block; The first row in a block will have value 0x00, and successive rows will increase by 0x20 each time.

The second section contains the actual column data. The data is always stored in the same order for rows in the same table, so if you know the table schema, you can easily locate the value for a given column. Whilst this section does not contain actual string data, it does provide the location of strings, relative to the start of the row.

The final section contains the actual string data (if there are any strings in the table schema). Each string starts with one byte indicating the number of bytes to follow. However, this length byte seems to be coded in a strange way (or am I missing something obvious?). To get the actual length, take the byte value, subtract 1, divide by 2, and then subtract 1 again.

Let’s look at an example for getting the name of a track. We know that, according to the track table schema, the two bytes at 0x80 hold the location of the track name string. This value may be 0xde00, in which case we then read the byte at that offset, which is the length byte. If say, this byte has a value of 0x25 (37), then we calculate (37-1)/2 - 1 = 17. So we read the next 17 bytes as a UTF8 string.

I can’t seem to find an index for looking up the location of rows, and rows don’t seem to have any data indicating the total row size. Therefore, to iterate over rows, you must first seek to the end of the current row (by finding the last string and it’s size). Rows are not tightly packed, and tend to be padded with null bytes (Need to confirm this as I think I had one instance where these bytes were not zero). Once you have found the end of a row, you can keep reading bytes whilst they are zero, or keep reading until you find a valid row header.

Also, you will find that there are often duplicate rows. If you edit data for a track in Recordbox, it will often add a new row to the .pdb file rather than modifying the existing one. This means that, in order to get a row, you have to find all of the rows with the same id first, and then use the last one.


# Table Schemas

## Tracks
| Location    | Data type | Description                          |
| --------    | --------- | -----------                          |
| 0x00	      | uint16    | Table ID = 0x24                      |
| 0x02	      | uint16	  | Count / Row ID (increases by 0x20)   |
| 0x04 - 0x07 |           | Unknown                              |
| 0x08        | uint32    | Sample Rate                          |
| 0x0c        | uint32    | Composer (Artist ID)                 |
| 0x10        | uint32    | File size (bytes)                    |
| 0x14        |	uint32	  | Track ID                             |
| 0x18 - 0x23 |	          | Unknown                              |
| 0x24        | uint32    | Original artist (Artist ID)          |
| 0x28 - 0x2b |           | Unknown                              |
| 0x2c        | uint32    | Remixer (Artist ID)                  |
| 0x30        | uint32    | Bitrate (kbps)                       |
| 0x34        | uint32    | Track number                         |
| 0x38 - 0x3f |           |	Unknown                              |
| 0x40        | uint32    | Album (Album ID)                     |
| 0x44        | uint32    | Artist (Artist ID)                   |
| 0x48 - 0x4b |           | Unknown                              |
| 0x4c        |	uint16    |	Disc number                          |
| 0x4e        | uint16    | Play count                           |
| 0x50 - 0x53	|	          | Unknown                              |
| 0x54        | uint16    | Duration (seconds)                   |
| 0x56 - 0x5d |           | Unknown                              |
| 0x5e        |	uint16	  | ? (string location)                  |
| 0x60        |	uint16    |	Lyricist (string location)           |
| 0x62        |	uint16    |	? (string location)                  |
| 0x64	      | uint16    | ? (string location)                  |
| 0x66	      | uint16    | ? (string location)                  |
| 0x68	      | uint16	  | KUVO (string location)               |
| 0x6a	      | uint16	  | Public (string location)             |
| 0x6c	      | uint16	  | Autoload HotCue (string location)    |
| 0x6e	      | uint16	  | ? (string location)                  |
| 0x70	      | uint16	  | ? (string location)                  |
| 0x72	      | uint16	  | Date (string location)               |
| 0x74        | uint16	  | ? (string location)                  |
| 0x76	      | uint16	  | Mix Name (string location)           |
| 0x78	      | uint16	  | ? (string location)                  |
| 0x7a	      | uint16    | DAT file (string location)           |
| 0x7c	      | uint16	  | Date (string location)               |
| 0x7e	      | uint16	  | Comment (string location)            |
| 0x80	      | uint16	  | Track Name (string location)         |
| 0x82	      | uint16	  | ? (string location)                  |
| 0x84	      | uint16	  | File name (string location)          |
| 0x86	      | uint16	  | File path (string location)          |

## Artists
Todo...

## Albums
Todo...


# Code Examples
See example-parse for a very basic example of how to parse a pdb file.
