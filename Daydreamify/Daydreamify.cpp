// Daydreamify.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*
    These are the bytes we need to add to the end of the file.
	There are 30 of them in total, so we also need to increase the varint
	at the start of the file that tells the length of the remainder
	of the file by 30.



	9a 91 60
	1a
	08 01
	12 0a
	0d 00 00 00 00 15 f4 fd d4 3c
	12 0a
	0d 00 00 00 00 15 f4 fd d4 bc
*/

void varint_write(std::ofstream *out, uint32_t varint) {
	uint8_t outbytes[5];
	uint8_t startbyte = 4;
	for (int i = 0; i < 5; i++) outbytes[i] = 0;
//	std::cout << "Varint before stripping byte: " << (unsigned int) varint << std::endl;
	outbytes[startbyte] = varint & 0x7f;
//	std::cout << "Stripped byte: " << (unsigned int) outbytes[startbyte] << std::endl;
	varint = varint >> 7;
//	std::cout << "Varint after stripping byte: " << (unsigned int) varint << std::endl;
	while (varint > 0) {
		outbytes[--startbyte] = (varint & 0x7f) | 0x80;
//		std::cout << "Stripped byte: " << (unsigned int)outbytes[startbyte] << std::endl;
		varint = varint >> 7;
//		std::cout << "Varint after stripping byte: " << (unsigned int)varint << std::endl;
	}
//	std::cout << "Startbyte: " << (unsigned int) startbyte << std::endl;

	for (int i = startbyte; i < 5; i++) {
//		std::cout << "Writing Varint Byte" << std::endl;
		out->write((char*)&outbytes[i], 1);
	}
}



int main()
{
	const uint8_t header_length = 7;
	uint8_t header[header_length];
	uint32_t length = 0;
	uint8_t *contents = NULL;

	std::ifstream params_in;
	std::ofstream params_out;
	const uint8_t appendix_length = 30;
	const uint8_t appendix[appendix_length] = {
		0x9a, 0x91, 0x60,
		0x1a,
		0x08, 0x01,
		0x12, 0x0a,
		0x0d, 0x00, 0x00, 0x00, 0x00, 0x15, 0xf4, 0xfd, 0xd4, 0x3c,
		0x12, 0x0a,
		0x0d, 0x00, 0x00, 0x00, 0x00, 0x15, 0xf4, 0xfd, 0xd4, 0xbc
	};

	params_in.open("current_device_params", std::ifstream::in | std::ios::binary);
	if (!params_in) {
		std::cerr << "Error: current_device_params file not found" << std::endl;
		return -1;
	}
	
	params_out.open("new_device_params", std::ios::out | std::ios::binary);
	if (!params_out) {
		std::cerr << "Error: could not create new_device_params file" << std::endl;
		return -1;
	}

	// First, read 7-byte header at the start of the file.
	params_in.read((char *)header, header_length);

	if (params_in.eof()) {
		std::cerr << "Error: premature EOF in current_device_params" << std::endl;
		return -1;
	}


	// Next, read the Varint that tells the length of the remainder of the file.
	unsigned int charcount = 0;
	while (!params_in.eof()) {
		uint8_t current_byte;

		params_in.read((char *)&current_byte, 1);

//		std::cout << "Varint Byte " << std::dec << charcount++ << ": " << std::hex << std::setfill('0') << std::setw(2) << (unsigned int) current_byte << std::endl;
		
		length = length << 7;  // Left shift current length by 7 bits
		length += (current_byte & 0x7f);  // Add currchar
		if (!(current_byte & 0x80)) // high bit is not set
//			std::cout << "Varint complete" << std::endl;
			break;  // leave while loop
	}
	if (params_in.eof()) {
		std::cerr << "Error: premature EOF in current_device_params" << std::endl;
		return -1;
	}
	if (length == 0) {
		std::cerr << "Error: expected file length, returned 0 instead" << std::endl;
		return -1;
	}
	if (length > 2000) {
		std::cerr << "Error: expected a small file, got projected length of " << length << " instead" << std::endl;
		return -1;
	}

	// Allocate storage for the remainder of the file.
	contents = (uint8_t*)malloc(length);
	if (!contents) {
		std::cerr << "Error: unable to allocate memory to read in file." << std::endl;
		return -1;
	}

	// Now read in the remainder of the file, and check that it matches the declared length.
	charcount = 0;
	while (!params_in.eof() && (charcount < length)) {
		uint8_t current_byte;
		params_in.read((char *)&current_byte, 1);

//		std::cout << "Byte " << std::dec << charcount << ": " << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)current_byte << std::endl;
		contents[charcount] = current_byte;
		++charcount;
	}
	if (charcount != length) { 
		std::cerr << "Error: unexpected EOF in input file. Charcount = " << charcount << ", expected length = " << length << std::endl;
		return -1;
	}

	/*
	for (unsigned int i = 0; i < length; ++i) {
		std::cout << "Stored byte " << i << ": " << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)contents[i] << std::endl;
	}
	*/

	params_in.close();

	// Done reading the input file!

	// Output the header.
	params_out.write((char *)header, header_length);

	// Output the increased varint.
	uint32_t output_length = length + appendix_length;
	varint_write(&params_out, output_length);

	params_out.write((char *) contents, length);

	// Output the appendix.
	params_out.write((char *)appendix, appendix_length);

	params_out.close();

	// Done writing the output file!


    return 0;
}

