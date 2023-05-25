// This project is made for the HAN University of Applied Sciences Embedded Vision Design and Machine Learning Minor
// Source code supplied from the institution is used, such as image_t struct, setBasicValue, etc. 
// However since the images can be represented as matrices, the logic could be used for any image
// Discrete cosine transform is a lossy but efficient technique for image compression. Hence there is a decrease in quality.
// By applying inverse discrete cosine transfrom to dct coeffients, and applying quantization, we can restore the image.

void reconstruct_image_dct(image_t *img)
{
    // Make sure image size is a multiple of 8 by adding padding
    int padded_cols = (img->cols + 7) / 8 * 8;
    int padded_rows = (img->rows + 7) / 8 * 8;

    // Create a new empty image based on the size of cols and rows of the input
    image_t *src = newBasicImage(padded_cols, padded_rows);
    // Clone the values from img
    for (int r = 0; r < img->rows; r++) {
        for (int c = 0; c < img->cols; c++) {
            setBasicPixel(src, c, r, getBasicPixel(img, c, r));
        }
    }

    // Create a new empty image based on the size of cols and rows of the input
    image_t *dst = newBasicImage(padded_cols, padded_rows);

    // A double array to hold DCT coefficient values for compression
    float coefficients[padded_rows][padded_cols];
    float ci, cj, dct1, sum, idct1;

    // Double array for quantization matrix
    int quant[8][8] = {
        {4, 3, 2, 4, 6, 10, 13, 15},
        {3, 3, 3, 5, 6, 14, 15, 14},
        {3, 3, 4, 6, 10, 14, 17, 14},
        {3, 4, 5, 7, 13, 22, 20, 15},
        {4, 5, 9, 14, 17, 27, 26, 19},
        {6, 9, 14, 16, 20, 26, 28, 23},
        {12, 16, 19, 22, 26, 30, 30, 25},
        {18, 23, 24, 24, 28, 25, 26, 25}
    };

    // Apply DCT to each 8x8 block
    for (int i = 0; i < padded_rows; i+=8) {
        for (int j = 0; j < padded_cols; j+=8) {
            // For each pixel in the 8x8 block
            for (int x = i; x < i+8 && x < padded_rows; x++) {
                for (int y = j; y < j+8 && y < padded_cols; y++) {
                    // Calculate DCT coefficients
                    sum = 0.0;
                    for (int u = i; u < i+8 && u < padded_rows; u++) {
                        for (int v = j; v < j+8 && v < padded_cols; v++) {
                            dct1 = (float)getBasicPixel(src, v, u) * cos((2*u+1) * x * PI / 16.0) * cos((2*v+1) * y * PI / 16.0);
                            sum += dct1;
                        }
                    }
                    ci = (x == 0) ? sqrt(1.0/8) : sqrt(2.0/8);
                    cj = (y == 0) ? sqrt(1.0/8) : sqrt(2.0/8);
                    // Quantize the DCT coefficients
                    coefficients[x][y] = ci * cj * sum / quant[x % 8][y % 8];
                }
            }
        }
    }

    // Apply IDCT to each 8x8 block
    for (int x = 0; x < padded_rows; x+=8) {
        for (int y = 0; y < padded_cols; y+=8) {
            for (int i = x; i < x+8 && i < padded_rows; i++) {
                for (int j = y; j < y+8 && j < padded_cols; j++) {
                    // Calculate the original pixel values from the DCT coefficients
                    sum = 0.0;
                    for (int u = x; u < x+8 && u < padded_rows; u++) {
                        for (int v = y; v < y+8 && v < padded_cols; v++) {
                            ci = (u == 0) ? sqrt(1.0/8) : sqrt(2.0/8);
                            cj = (v == 0) ? sqrt(1.0/8) : sqrt(2.0/8);
                            idct1 = ci * cj * coefficients[v][u] * cos((2*i+1) * u * PI / 16.0) * cos((2*j+1) * v * PI / 16.0);
                            sum += idct1;
                        }
                    }
                    // Multiply with the quantization matrix to get the original block
                    coefficients[i][j] = sum * quant[i % 8][j % 8];
                }
            }
        }
    }

    // Copy the transformed pixels to the destination image
    for(int r = 0 ; r < padded_rows; r++){
        for(int c =0 ; c < padded_cols; c++){
            setBasicPixel(dst, c, r, (basic_pixel_t)coefficients[r][c]);
        }
    }

    // Display the original and reconstructed images
    showImg(src, "original image");
    showImg(dst, "reconstructed image");

    // Clean up memory
    deleteImage(src);
    deleteImage(dst);
}
