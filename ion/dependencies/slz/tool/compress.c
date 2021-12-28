//***************************************************************************
// "compress.c"
// Compresses a raw blob into an SLZ file
//***************************************************************************
// Slz compression tool
// Copyright 2011, 2017 Javier Degirolmo
//
// This file is part of the slz tool.
//
// The slz tool is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// The slz tool is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the slz tool.  If not, see <http://www.gnu.org/licenses/>.
//***************************************************************************

// Required headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "compress.h"

//***************************************************************************
// write_word
// Writes a 16-bit value into a file
//---------------------------------------------------------------------------
// param outfile: output file
// param value: value to be written
// return: error code
//***************************************************************************

int write_word(FILE *outfile, const uint16_t value) {
	// Split value into bytes
	uint8_t temp[2] = { value >> 8, value & 0xFF };

	// Try to write into file
	if(fwrite(temp, 1, 2, outfile) < 2)
		return ERR_CANTWRITE;

	// Success!
	return ERR_NONE;
}

//***************************************************************************
// write_tribyte
// Writes a 24-bit value into a file
//---------------------------------------------------------------------------
// param outfile: output file
// param value: value to be written
// return: error code
//***************************************************************************

int write_tribyte(FILE *outfile, const uint32_t value) {
	// Split value into bytes
	uint8_t temp[3] = { value >> 16, value >> 8, value & 0xFF };

	// Try to write into file
	if(fwrite(temp, 1, 3, outfile) < 3)
		return ERR_CANTWRITE;

	// Success!
	return ERR_NONE;
}

//***************************************************************************
// stream_word
// Writes a 16-bit value into a stream, and advances the stream ptr
//---------------------------------------------------------------------------
// param buffer: output stream
// param data: value to be written
// return: written length
//***************************************************************************

int stream_word(uint8_t ** buffer, const uint16_t data)
{
	uint8_t temp[2] = { data >> 8, data & 0xFF };
	*(*buffer)++ = temp[0];
	*(*buffer)++ = temp[1];
	return 2;
}

//***************************************************************************
// stream_tribyte
// Writes a 24-bit value into a stream, and advances the stream ptr
//---------------------------------------------------------------------------
// param buffer: output stream
// param data: value to be written
// return: written length
//***************************************************************************

int stream_tribyte(uint8_t ** buffer, const uint32_t data)
{
	uint8_t temp[3] = { data >> 16, data >> 8, data & 0xFF };
	*(*buffer)++ = temp[0];
	*(*buffer)++ = temp[1];
	*(*buffer)++ = temp[2];
	return 3;
}

//***************************************************************************
// stream_buffer
// Writes a buffer value into a stream, and advances the stream ptr
//---------------------------------------------------------------------------
// param buffer: output stream
// param data: buffer to be written
// param data_size: buffer size
// return: written length
//***************************************************************************

int stream_buffer(uint8_t** buffer, const uint8_t* data, int data_size)
{
	memcpy(*buffer, data, data_size);
	*buffer += data_size;
	return data_size;
}

//***************************************************************************
// compress
// Reads an uncompressed blob from a file and outputs SLZ compressed data
//---------------------------------------------------------------------------
// param infile: input file
// param outfile: output file
// param format: SLZ variant in use
// return: error code
//***************************************************************************

int compress_file(FILE *infile, FILE *outfile, int format) {
   // THIS SHOULDN'T HAPPEN
   // (also this check is to help the compiler optimize)
   if (format != FORMAT_SLZ16 && format != FORMAT_SLZ24)
      return ERR_UNKNOWN;

   // To store error codes
   int errcode;

   // Get filesize (sorry, we have to seek here!)
   if (fseek(infile, 0, SEEK_END)) return ERR_CANTREAD;
   long eof_pos = ftell(infile);
   if (eof_pos == -1) return ERR_CANTREAD;
   size_t filesize = (size_t)(eof_pos);
   if (fseek(infile, 0, SEEK_SET)) return ERR_CANTREAD;

   // Empty?
   if (filesize == 0) {
      if (format == FORMAT_SLZ16)
         errcode = write_word(outfile, 0);
      else
         errcode = write_tribyte(outfile, 0);
      return errcode;
   }

   // Allocate memory to store blob
   uint8_t *blob = (uint8_t *) malloc(sizeof(uint8_t) * filesize);
   if (blob == NULL) return ERR_NOMEMORY;

   // Load input file into memory
   if (fread(blob, 1, filesize, infile) < filesize) {
      free(blob);
      return ERR_CANTREAD;
   }

   // Allocate memory to store compressed blob
   uint8_t *compblob = (uint8_t *)malloc(sizeof(uint8_t) * filesize + 2);
   if(compblob == NULL) return ERR_NOMEMORY;

   // Output buffer start
   uint8_t *outblob = compblob;

   // Compress data
   int compressed_size = errcode = compress_memory(blob, compblob, (int)filesize, (int)filesize + 2, format);
   if(errcode <= ERR_UNKNOWN)
   {
	   free(blob);
	   free(compblob);
	   return errcode;
   }

   // Write data
   if(fwrite(outblob, 1, compressed_size, outfile) < compressed_size)
   {
	   free(blob);
	   free(compblob);
	   return ERR_CANTWRITE;
   }

   // We don't need the blobs anymore
   free(blob);
   free(outblob);

   // Success!
   return ERR_NONE;
}

//***************************************************************************
// compress
// Compresses a provided memory blob and outputs SLZ compressed data
//---------------------------------------------------------------------------
// param in: input buffer
// param out: output buffer
// param insize: input buffer size
// param outsize: output buffer size (must be at least insize+2)
// param format: SLZ variant in use
// return: error code, or compressed size
//***************************************************************************
int compress_memory(uint8_t * in, uint8_t * out, int insize, int outsize, int format)
{
	if(outsize < insize + 2)
		return ERR_BUFFERTOOSMALL;

	// Too large for SLZ?
	if(insize > 0xFFFF && format == FORMAT_SLZ16)
		return ERR_TOOLARGE16;
	else if(insize > 0xFFFFFF && format == FORMAT_SLZ24)
		return ERR_TOOLARGE24;

	// Compressed size
	int compressed_size = 0;

	// Write uncompressed size
	if(format == FORMAT_SLZ16)
		compressed_size += stream_word(&out, (uint16_t)(insize));
	else
		compressed_size += stream_tribyte(&out, (uint32_t)(insize));

	// To store token data
	uint8_t tokens = 0;
	int num_tokens = 0;

	// Buffer to store the compressed data (we need to buffer this because the
	// tokens need to be written first)
	uint8_t buffer[0x10];
	int bufsize = 0;

	// Scan through all bytes
	size_t pos = 0;
	while(pos < insize) {
		// Used to store token information
		int compressed = 0;
		uint16_t dist = 3;
		uint8_t len = 2;

		// Make room for next token
		tokens <<= 1;
		num_tokens++;

		// First byte is always uncompressed
		// Also if there are too few bytes left, we can't compress them, so
		// don't bother scanning those either
		if(pos > 0 && insize - pos >= 3) {
			// Determine maximum distance to look for
			uint16_t max_dist = (uint16_t)(pos > 0x1002 ? 0x1002 : pos);

			// Determine maximum length to check for (to avoid overflow issues)
			uint8_t max_len = (uint8_t)(insize - pos > 18 ? 18 : insize - pos);

			// Pointer to the strings we're going to compare
			// Making them pointers to help the compiler optimize
			const uint8_t *target = &in[pos];
			const uint8_t *other = &in[pos - max_dist];

			// Scan for a possible match
			size_t curr_dist;
			for(curr_dist = max_dist; curr_dist >= 3; curr_dist--, other++)
			{
				// To avoid wasting time on matches that WILL fail...
				if(*other != *target)
					continue;

				// Check all lengths that are larger than the longest string we
				// found so far (don't bother with shorter strings as that'd be
				// a waste)
				uint8_t curr_len;
				for(curr_len = max_len; curr_len > len; curr_len--) {
					// Did we find a match? (if so, don't bother with smaller
					// string, also mark the token as compressable)
					if(!memcmp(other, target, curr_len)) {
						dist = (uint16_t)(curr_dist);
						len = curr_len;
						compressed = 1;
						break;
					}
				}
			}
		}

		// Compressed token?
		if(compressed) {
			// Skip repeated string in the blob
			pos += len;

			// Store contents of the token
			dist -= 3;
			len -= 3;
			buffer[bufsize] = dist >> 4;
			buffer[bufsize + 1] = dist << 4 | len;
			bufsize += 2;

			// Mark token as compressed
			tokens |= 1;
		}

		// Uncompressed token?
		else {
			buffer[bufsize] = in[pos];
			pos++;
			bufsize++;
		}

		// Huh, done with this group of tokens?
		if(num_tokens == 8) {
			// Write token types
			compressed_size += stream_buffer(&out, &tokens, 1);

			// Write buffered data
			compressed_size += stream_buffer(&out, buffer, bufsize);

			// Reset tokens and buffer
			tokens = 0;
			num_tokens = 0;
			bufsize = 0;
		}
	}

	// If there are any tokens left, make sure they're written too
	if(num_tokens < 8) {
		// Make sure token types are MSB aligned
		tokens <<= 8 - num_tokens;

		// Write token types
		// Write token types
		compressed_size += stream_buffer(&out, &tokens, 1);

		// Write buffered data
		compressed_size += stream_buffer(&out, buffer, bufsize);
	}

	return compressed_size;
}
