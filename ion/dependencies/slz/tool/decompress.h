//***************************************************************************
// "decompress.h"
// Header file for "decompress.c"
//***************************************************************************
// Slz compression tool
// Copyright 2011 Javier Degirolmo
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

#ifndef DECOMPRESS_H
#define DECOMPRESS_H

// Required headers
#include <stdio.h>
#include <stdint.h>

// Function prototypes
#if defined (__cplusplus)
extern "C" {
#endif

int decompress(FILE *, FILE *, int);

int read_word(FILE *, uint16_t *);
int read_tribyte(FILE *, uint32_t *);

#if defined (__cplusplus)
}
#endif

#endif
