#pragma pack_matrix(row_major)

// Utility Functions
// -----------------

// Get the pixel position from a screen quad's UVs.
float2 GetPixelCoords(float2 ScreenUVs, float2 ScreenResolution)
{
    return float2(ScreenUVs * ScreenResolution);
}
