#include <stdio.h>
#include <string.h>
#include <libgen.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void print_info(const char* image_file, const int width, const int height, const size_t ascii_size) {
    printf("%s:\n\twidth: %d\n\theight: %d\n", image_file, width, height);

    if(ascii_size >= (1024 * 1024)) {
        printf("%.2lf MiB for ASCII image (allocated)\n", ascii_size / (1024.0 * 1024.0));
    } else if(ascii_size >= 1024) {
        printf("%.2lf KiB for ASCII image (allocated)\n", ascii_size / 1024.0);
    } else {
        printf("%zu bytes for ASCII image (allocated)\n", ascii_size);
    }
}

void write_ascii_image_to_file(const char* output_file, const uint8_t* ascii_image) {
    FILE* output = fopen(output_file, "w");
    fputs(ascii_image, output);
    fclose(output);
}

uint8_t* get_ascii_from_image(const char* image_file, const int width, const int height, const int components, const uint8_t* image) {
    const size_t ascii_size = (width + 1) * height + 1;
    uint8_t* ascii_image = (uint8_t*)malloc(ascii_size);
    if(ascii_image == NULL) {
        return NULL;
    }

    print_info(image_file, width, height, ascii_size);

    const char* gray_scale = " .:-=+*#%@";
    const size_t scale_len = strlen(gray_scale);
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            const int pixel_index = components * (i * width + j);
            const uint8_t lum = 0.299f * image[pixel_index] + 0.587f * image[pixel_index + 1] + 0.114f * image[pixel_index + 2];
            ascii_image[i * (width + 1) + j] = gray_scale[(lum * scale_len) / 256];
        }
        ascii_image[i * (width + 1) + width] = '\n';
        
    }

    ascii_image[(width + 1) * height] = '\0';

    return ascii_image;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("usage: %s <image>\n", argv[0]);
        return 1;
    }

    int width, height, components;
    uint8_t* image = (uint8_t*)stbi_load(argv[1], &width, &height, &components, 0);
    if(image == NULL) {
        fprintf(stderr, "couldnt load image %s; error: %s\n", argv[1], stbi_failure_reason());
        return 1;
    }

    uint8_t* ascii_image = get_ascii_from_image(argv[1], width, height, components, image);
    if(ascii_image == NULL) {
        fprintf(stderr, "couldnt generate ASCII image/art from the image.\n");
        stbi_image_free(image);
        return 1;
    }

    char* output_file = strdup(basename(argv[1]));
    *strchr(output_file, '.') = '\0';
    strcat(output_file, ".txt");

    write_ascii_image_to_file(output_file, ascii_image);
    free(output_file);

    free(ascii_image);
    return 0;
}