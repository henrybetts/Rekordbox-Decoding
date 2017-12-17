//
//  schema.h
//  example-parse
//
//  Created by Henry Betts on 17/12/2017.
//  Copyright © 2017 Henry Betts. All rights reserved.
//

#ifndef schema_h
#define schema_h

#include <stdint.h>

struct PDBBlockHeader {
    unsigned char x00[4];
    uint32_t index;
    unsigned char x08[16];
    uint8_t numRows;
    unsigned char x19[15];
};

#define PDBTrackID 0x0024

struct PDBTrack {
    uint32_t header;
    unsigned char x04[4];
    uint32_t sampleRate;
    unsigned char x0c[8];
    uint32_t id;
    unsigned char x18[28];
    uint16_t trackNumber;
    unsigned char x36[22];
    uint16_t discNumber;
    unsigned char x4[16];
    
    uint16_t x5e;
    uint16_t str_lyricist;
    uint16_t x62;
    uint16_t x64;
    uint16_t x66;
    uint16_t str_KUVO;
    uint16_t x6a;
    uint16_t x6c;
    uint16_t x6e;
    uint16_t x70;
    uint16_t str_date;
    uint16_t x74;
    uint16_t str_mixName;
    uint16_t x78;
    uint16_t str_dataFilepath;
    uint16_t str_date2;
    uint16_t str_comment;
    uint16_t str_name;
    uint16_t x82;
    uint16_t str_filename;
    uint16_t str_filepath;
};

#endif /* schema_h */
