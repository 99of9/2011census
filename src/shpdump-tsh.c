/******************************************************************************
 * $Id: shpdump.c,v 1.18 2011-07-24 03:05:14 fwarmerdam Exp $
 *
 * Project:  Shapelib-tsh
 * Purpose:  Application for dumping contents of a 2-d shapefile to 
 *           the terminal in svg form.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *           Toby Hudson, tobyyy@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 1999, Frank Warmerdam & 2012 Toby Hudson
 *
 * This software is available under the following "MIT Style" license,
 * or at the option of the licensee under the LGPL (see LICENSE.LGPL).  This
 * option is discussed in more detail in shapelib.html.
 *
 * --
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log: shpdump.c,v $
 * Revision 1.18  2011-07-24 03:05:14  fwarmerdam
 * use %.15g for formatting coordiantes in shpdump
 *
 */

#include <string.h>
#include <stdlib.h>
#include "shapefil.h"

SHP_CVSID("$Id: shpdump.c,v 1.18 2011-07-24 03:05:14 fwarmerdam Exp $")


#define         MAXFILENAME 1000
double 	        adfMinBound[4], adfMaxBound[4];
SHPHandle	hSHP;
DBFHandle	hDBF;
int		nEntities;


void plain_characters(char str[1000]) {
  int i=0;
  while(str[i]!='\0') {
    if ((str[i]<48)||((str[i]>57)&&(str[i]<65))||((str[i]>90)&&(str[i]<97))||(str[i]>123)) {
      str[i] = '_';
    } 
    i++;
  }
}

void detailed_svg(int argc, char ** argv) {
  FILE *fp;
  int i, iPart;
  char szFormat[32];
  char	szTitle[12];
  char label[1000];
  int nWidth, nDecimals;
  //DBFFieldType	eType;

  DBFGetFieldInfo( hDBF, 1, szTitle, &nWidth, &nDecimals );
  sprintf(szFormat, "%%d\t%%-%ds\n", nWidth );

  fp = fopen("detailed_shape.svg", "w");
  
  fprintf(fp, "<?xml version=\"1.0\" standalone=\"no\"?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n    \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n <svg width=\"15cm\" height=\"%.15gcm\" viewBox=\"%.15g %.15g %.15g %.15g\"\n xmlns=\"http://www.w3.org/2000/svg\"\n xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n xmlns:ev=\"http://www.w3.org/2001/xml-events\" version=\"1.1\">\n  <desc>SVG version of %s</desc>\n\n", 15.0*(adfMaxBound[1]-adfMinBound[1])/(adfMaxBound[0]-adfMinBound[0]), adfMinBound[0], -adfMaxBound[1], adfMaxBound[0]-adfMinBound[0], -adfMinBound[1]+adfMaxBound[1], argv[1]);

  fprintf(fp, "<g fill=\"grey\" stroke=\"black\" stroke-width=\"0.001\" stroke-linejoin=\"round\">\n");

  for( i = 0; i < nEntities; i++ )
    {
      int		j;
      SHPObject	*psShape;
      
      if (!DBFIsAttributeNULL(hDBF,i,4)) {
	if (DBFReadDoubleAttribute(hDBF,i,4)>0.00001) {
	  printf(szFormat, i, DBFReadStringAttribute( hDBF, i, 1));
	}
      }

      psShape = SHPReadObject( hSHP, i );
      
      if( psShape == NULL )
        {
	  fprintf( stderr,
		   "Unable to read shape %d, terminating object reading.\n",
		   i );
	  break;
        }
      
      /*
	if( psShape->bMeasureIsUsed )
	printf( "\nShape:%d (%s)  nVertices=%d, nParts=%d\n"
	"  Bounds:(%.15g,%.15g, %.15g, %.15g)\n"
	"      to (%.15g,%.15g, %.15g, %.15g)\n",
	i, SHPTypeName(psShape->nSHPType),
	psShape->nVertices, psShape->nParts,
	psShape->dfXMin, psShape->dfYMin,
	psShape->dfZMin, psShape->dfMMin,
	psShape->dfXMax, psShape->dfYMax,
	psShape->dfZMax, psShape->dfMMax );
	else
	printf( "\nShape:%d (%s)  nVertices=%d, nParts=%d\n"
	"  Bounds:(%.15g,%.15g, %.15g)\n"
	"      to (%.15g,%.15g, %.15g)\n",
	i, SHPTypeName(psShape->nSHPType),
	psShape->nVertices, psShape->nParts,
	psShape->dfXMin, psShape->dfYMin,
	psShape->dfZMin,
	psShape->dfXMax, psShape->dfYMax,
	psShape->dfZMax );
      */

      fprintf(fp, "  <path fill=\"rgb(%d,%d,%d)\" d=\"M ", rand()%256, rand()%256, rand()%256);
      
      if( psShape->nParts > 0 && psShape->panPartStart[0] != 0 )
        {
	  fprintf( stderr, "panPartStart[0] = %d, not zero as expected.\n",
		   psShape->panPartStart[0] );
        }
      
      for( j = 0, iPart = 1; j < psShape->nVertices; j++ )
        {	  
	  if( iPart < psShape->nParts
	      && psShape->panPartStart[iPart] == j )
            {
	      iPart++;
	      
	      /* start a new polygon, either an island or a cutout.  It would be nice to make the cutouts carve a hole in the underlying polygon */
	      //printf("\" />\n  <polygon fill=\"red\" stroke=\"blue\" stroke-width=\"0.003\" \n            points=\"");
	      fprintf(fp, " z M ");
            }
	  
	  //printf(" %.11g,%.11g", psShape->padfX[j], -psShape->padfY[j]);
	  fprintf(fp, "%.8g %.8g ", psShape->padfX[j], -psShape->padfY[j]);
	  
        }
      
      fprintf(fp, " z\" />\n");
      
      sprintf(label, "%s", DBFReadStringAttribute(hDBF,i,1));

      plain_characters(label);
    
      //fprintf(fp, "<text x=\"%.8g\" y=\"%.8g\" font-family=\"Verdana\" font-size=\"0.3\" fill=\"blue\" > %s </text>\n", 0.5*(psShape->dfXMin+psShape->dfXMax), -0.5*(psShape->dfYMin+psShape->dfYMax), label);
	 
      SHPDestroyObject( psShape );
    } 
  
  fprintf(fp, "</g>\n");
  fprintf(fp, "</svg>\n");
  fclose(fp);
  
}

int main( int argc, char ** argv )

{
    int		nShapeType;
    char        file_name_shp[MAXFILENAME];
    char        file_name_dbf[MAXFILENAME];

    if( argc != 2 )
      {
        printf( "shpdump shp_file_stem\n" );
        exit( 1 );
      }
    
    sprintf(file_name_shp, "%s.svg", argv[1]);
    sprintf(file_name_dbf, "%s.dbf", argv[1]);
    
    hSHP = SHPOpen( file_name_shp, "rb" );
    hDBF = DBFOpen( file_name_dbf, "rb" );
    
    if( hSHP == NULL )
      {
        printf( "Unable to open:%s\n", file_name_shp );
        exit( 1 );
      }
    if( hDBF == NULL )
      {
	printf( "DBFOpen(%s,\"r\") failed.\n", file_name_dbf );
	exit( 2 );
      }
    
    SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );
    
    printf("There are %d records with %d fields in the dbf table, and %d entities in the shp file \n", DBFGetRecordCount(hDBF), DBFGetFieldCount(hDBF), nEntities);
    
    if (DBFGetRecordCount(hDBF)!=nEntities) {
      fprintf(stderr, "DBF records and SHP entities don't match\n");
      exit(3);
    }

    detailed_svg(argc, argv);
    
    SHPClose( hSHP );
    
    exit( 0 );
}
