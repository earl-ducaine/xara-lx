 /* statlist.cpp
 *
 * Copyright (C) © 2010 E. Geldhof
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Author:  E. Geldhof <ed44@xs4all.nl>
 */

struct statnode statlist[]={
	{"GDraw_GetSvnVersion", 0},/*	0 */
	{"GDraw_ClearLastError", 0},/*1*/
	{"GDraw_GetLastError", 0},/*2*/
	{"GDraw_GetLastErrorText", 0},/*3*/
	{"GDraw_Initialise", 0},/*4*/
	{"GDraw_CopyContext", 0},/*5*/
	{"GColour_GetGraduationTableSize", 0},/*6*/
	{"GColour_GetGraduationTableLength", 0},/*7*/
	{"GColour_GetLongGraduationTableSize" , 0},/*8*/
	{"GColour_GetLongGraduationTableLength", 0},/*9*/
	{"GColour_GetTransparentGraduationTableSize", 0},/*10*/
	{"GColour_GetTransparentGraduationTableLength", 0},/*11*/
	{"GColour_GetLongTransparentGraduationTableSize", 0},/*12*/
	{"GColour_GetLongTransparentGraduationTableLength", 0},/*13*/
	{"GDraw_SetMemoryHandlers", 0},/*14*/
	{"GDraw_SetStackLimit", 0},/*15*/
	{"GDraw_SetStackSize", 0},/*16*/
	{"GDraw_Terminate", 0},/*17*/
	{"GDraw_Clean", 0},/*18*/
	{"GDraw_ComparePalettes", 0},/*19*/
	{"GColour_SelectPalette", 0},/*20*/
	{"GColour_InitialiseWithPalette", 0},/*21*/
	{"GColour_SetDitherStyle", 0},/*22*/
	{"GColour_SetTileSmoothingFlag", 0},/*23*/
	{"GColour_SetTileFilteringFlag", 0},/*24*/
	{"GBitmap_SetMaxFilterSize", 0},/*25*/
	{"GBitmap_Sharpen", 0},/*26*/
	{"GBitmap_Blur", 0},/*27*/
	{"GColour_SetHalftoneOrigin", 0},/*28*/
	{"GColour_SetColour", 0},/*29*/
	{"GColour_SetSolidColour", 0},/*30*/
	{"GColour_SetWordColour", 0},/*31*/
	{"GColour_SetInvert", 0},/*32*/
	{"GColour_SetTransparency", 0},/*33*/
	{"GColour_SetTransparencyLookupTable", 0},/*34*/
	{"GColour_ReturnBrush", 0},/*35*/
	{"GColour_ReturnBrushRGB", 0},/*36*/
	{"GColour_AddToGraduationTable", 0},/*37*/
	{"GColour_BuildGraduationTable", 0},/*38*/
	{"GColour_BuildGraduationTable32", 0},/*39*/
	{"GColour_BuildTransparencyTable", 0},/*40*/
	{"GColour_SetGraduation", 0},/*41*/
	{"GColour_SetGraduation4", 0},/*42*/
	{"GColour_Set3WayGraduation", 0},/*43*/
	{"GColour_Set3WayGraduation4", 0},/*44*/
	{"GColour_Set4WayGraduation", 0},/*45*/
	{"GColour_Set4WayGraduation4", 0},/*46*/
	{"GColour_SetGourand", 0},/*47*/
	{"GColour_SetTransparentGraduation", 0},/*48*/
	{"GColour_SetTransparentGraduation4", 0},/*49*/
	{"GColour_SetTransparent3WayGraduation", 0},/*50*/
	{"GColour_SetTransparent3WayGraduation4", 0},/*51*/
	{"GColour_SetTransparent4WayGraduation", 0},/*52*/
	{"GColour_SetTransparent4WayGraduation4", 0},/*53*/
	{"GBitmap_SetBrightness", 0},/*54*/
	{"GBitmap_SetContrast", 0},/*55*/
	{"GBitmap_SetGamma", 0},/*56*/
	{"GBitmap_SetPostGamma", 0},/*57*/
	{"GBitmap_SetSaturation", 0},/*58*/
	{"GBitmap_SetContone", 0},/*59*/
	{"GBitmap_SetBias", 0},/*60*/
	{"GBitmap_SetGain", 0},/*61*/
	{"GBitmap_SetInputRange", 0},/*62*/
	{"GBitmap_SetOutputRange", 0},/*63*/
	{"GColour_SetTilePattern", 0},/*64*/
	{"GColour_SetTilePattern4", 0},/*65*/
	{"GColour_SetTransparentTilePattern", 0},/*66*/
	{"GColour_SetTransparentTilePattern4", 0},/*67*/
	{"GColour_ConvertBitmap", 0},/*68*/
	{"GColour_SetConversionPalette", 0},/*69*/
	{"GColour_SetMaxDiffusionError", 0},/*70*/
	{"GColour_ScaleBitmap", 0},/*71*/
	{"GColour_SetGreyConversionValues", 0},/*72*/
	{"GColour_ConvertHSVtoRGB", 0},/*73*/
	{"GColour_ConvertRGBtoHSV", 0},/*74*/
	{"GColour_SetSeparationTables", 0},/*75*/
	{"GColour_SetBitmapConversionTable", 0},/*76*/
	{"GDraw_SetDIBitmap", 0},/*77*/
	{"GDraw_SetInvertedDIBitmap", 0},/*78*/
	{"GDraw_SetMatrix", 0},/*79*/
	{"GDraw_MaxScale", 0},/*80*/
	{"GDraw_GetMaxBitmapWidth", 0},/*81*/
	{"GDraw_GetMaxBitmapDepth", 0},/*82*/
	{"GDraw_SetAntialiasFlag", 0},/*83*/
	{"GDraw_SetAntialiasQualityFlag", 0},/*84*/
	{"GDraw_SetDashAdjustmentFlag", 0},/*85*/
	{"GDraw_SetHintingFlag", 0},/*86*/
	{"GDraw_SetFlatness", 0},/*87*/
	{"GDraw_SetMiterLimit", 0},/*88*/
	{"GDraw_FillRectangle", 0},/*89*/
	{"GDraw_FillPoint", 0},/*90*/
	{"GDraw_FillPath", 0},/*91*/
	{"GDraw_HintPath", 0},/*92*/
	{"GDraw_StrokePath", 0},/*93*/
	{"GDraw_StrokePathToPath", 0},/*94*/
	{"GDraw_CalcStrokeBBox", 0},/*95*/
	{"GDraw_IsOverlap", 0},/*96*/
	{"GDraw_IsStrokeOverlap", 0},/*97*/
	{"GDraw_GetStatistics", 0},/*98*/
	{"GDraw_MakeRegion", 0},/*99*/
	{"GDraw_MakeUnclippedRegion", 0},/*100*/
	{"GDraw_FillRegion", 0},/*101*/
	{"GDraw_ClipRectangle", 0},/*102*/
	{"GDraw_DeviceClipRectangle", 0},/*103*/
	{"GDraw_GetDeviceClipRectangle", 0},/*104*/
	{"GDraw_ClipRegion", 0},/*105*/
	{"GDraw_GetClipRegion", 0},/*106*/
	{"GDraw_CalcSimpleBBox", 0},/*107*/
	{"GDraw_CalcBBox", 0},/*108*/
	{"GDraw_TransformPath", 0},/*109*/
	{"GDraw_ScrollBitmap", 0},/*110*/
	{"GDraw_ClearChangedBBox", 0},/*111*/
	{"GDraw_GetChangedBBox", 0},/*112*/
	{"GDraw_SetChangedBBox", 0},/*113*/
	{"GDraw_SetBevelContrast", 0},/*114*/
	{"GDraw_SetBevelLightness", 0},/*115*/
	{"GDraw_SetBevelDarkness", 0},/*116*/
	{"GDraw_TranslateBevelValue", 0},/*117*/
	{"GBitmap_PlotTile", 0},/*118*/
	{"GBitmap_PlotTile4", 0},/*119*/
	{"ClipPathToPath", 0},/*120*/
	{"GenStroke::StrokePathToPath", 0},/*121*/
	{"GenStroke::Close", 0},/*122*/
	{"GenPathContour::ContourPath", 0},/*123*/
	{"GenBevelFaces::BevelPath" ,0},/*124*/
	{"GDraw2_SetDIBitmap", 0},/*125*/
	{"GDraw2_FillTriangle", 0},/*126*/
	{"GDraw2_FillTrapezium", 0},/*127*/
	{"GenerateWallShadow", 0},/*128*/
	{"GenerateFloorShadow", 0},/*129*/
	{"ContourBitmap", 0}/*130*/
};

#define MAX_STATNODE (sizeof(statlist)/sizeof(statnode))
