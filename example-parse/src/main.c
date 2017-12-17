//
//  main.c
//  example-parse
//
//  Created by Henry Betts on 17/12/2017.
//  Copyright © 2017 Henry Betts. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "schema.h"

uint8_t pdb_getStringLength(unsigned char* row, uint16_t location){
    return (row[location] - 1) / 2 - 1;
}

char* pdb_copyString(unsigned char* row, uint16_t location){
    uint8_t length = pdb_getStringLength(row, location);
    char* str = malloc(length + 1);
    memcpy(str, &row[location+1], length);
    str[length] = 0x00;
    return str;
}

int main(int argc, const char * argv[]) {
    
    if (argc < 2){
        printf("No file specified.");
        return 1;
    }
    
    FILE *file;
    unsigned char buffer[4096];
    size_t nread;
    
    //open file
    file = fopen(argv[1], "r");
    if (!file){
        printf("Could not open file.\n");
        return 1;
    }
    
    printf("Opened file %s\n", argv[1]);
    
    //skip first block
    fseek(file, 4096, SEEK_SET);
    
    //read each block into buffer
    while ((nread = fread(buffer, 1, 4096, file)) == 4096) {
        
        //get block header
        struct PDBBlockHeader blockHeader;
        memcpy(&blockHeader, buffer, 40);
        
        //for now, let's just get blocks that contain tracks
        if (*(uint16_t*)(&buffer[40]) == PDBTrackID){
            
            int currentRowIdx = 0;
            int currentPos = 40;
            
            //iterate over rows
            while (currentRowIdx < blockHeader.numRows && currentPos < 4096){
                
                //skip null padding bytes
                if (*(uint16_t*)(&buffer[currentPos]) != PDBTrackID){
                    currentPos++;
                    continue;
                }
                
                //read track data
                struct PDBTrack track;
                memcpy(&track, &buffer[currentPos], sizeof(track));
                
                char* trackName = pdb_copyString(&buffer[currentPos], track.str_name);
                
                printf("Found track: %s (id=%i)\n", trackName, track.id);
                
                free(trackName);
                
                //go to the end of the row (position of last string plus it's size)
                currentPos += track.str_filepath + pdb_getStringLength(&buffer[currentPos], track.str_filepath) + 1;
                currentRowIdx++;
                
            }
            
        }
        
    }
    
    printf("End of file.\n");
    
    fclose(file);
    
    return 0;
}
