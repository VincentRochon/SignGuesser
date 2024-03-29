#include "Distribution_Uniforme.h"

Distribution_Uniforme::Distribution_Uniforme(int windowSize)
	:mWindowSize(windowSize)
{
}

std::vector<float> Distribution_Uniforme::getKernel()
{
	size_t windowWidth = (mWindowSize * 2 + 1);
	size_t window = (windowWidth * windowWidth);
	
	std::vector<float> ConvolutionArray(window, (1.0f / window));

	return ConvolutionArray;
}

bool Distribution_Uniforme::ProcessImage(std::vector<QImage> const &imageIn, std::vector<QImage>& imageOut)
{

	if (imageIn.size() != imageOut.size()) {
		return false; // invalid format of either vectors
	}

	size_t windowWidth = (mWindowSize * 2 + 1);
	size_t window = (windowWidth * windowWidth);
	int posTracker{ 0 };
	unsigned char maxRed{};
	unsigned char maxBlue{};
	unsigned char maxGreen{};
	unsigned char r{};
	unsigned char g{};
	unsigned char b{};
	int c{};
	float uniformeValue{ 1.0f / window };

	auto imgIn{ imageIn.data() };
	auto imgOut{ imageOut.data() };

	for (size_t i = 0; i < imageIn.size(); i++) {


		//QImage im(*img);

		int imgWidth{ imgOut->width() };
		int imgHeight{ imgOut->height() };

		int* curPix{ reinterpret_cast<int*>(imgOut->bits()) };
		int* endPix{ curPix + imgWidth * imgHeight };
		const int* curViewPix{ reinterpret_cast<const int*>(imgIn->bits()) };

		curViewPix += imgWidth * mWindowSize;
		// endViewPix -= imageWidth * mWindowSize;
		curPix += imgWidth * mWindowSize; // skip lines to prevent overflow
		endPix -= imgWidth * mWindowSize; // remove last lines prevent overflow for treatment

		while (curPix < endPix) {

			if (posTracker > mWindowSize&& posTracker < (imgWidth - mWindowSize))
			{

				const int* startPix{ curViewPix - imgWidth * mWindowSize + mWindowSize }; // start of the window of pixels

				maxRed = 0;
				maxGreen = 0;
				maxBlue = 0;


				for (size_t i = 0; i < windowWidth; ++i)
				{
					for (size_t j = 0; j < windowWidth; ++j)
					{
						c = *startPix;

						r = static_cast<unsigned char>((c & 0x00'FF'00'00) >> 16);
						g = static_cast<unsigned char>((c & 0x00'00'FF'00) >> 8);
						b = static_cast<unsigned char>((c & 0x00'00'00'FF) >> 0);

						maxRed += r * uniformeValue;
						maxGreen += g * uniformeValue;
						maxBlue += b * uniformeValue;

						++startPix;
					}
					startPix += imgWidth - windowWidth;// skip 1 line
				}

				*curPix = (maxRed << 16) | (maxGreen << 8) | (maxBlue << 0) | 0xFF'00'00'00;;
			}

			if (posTracker == imgWidth) {
				posTracker = 0;
			}

			++curViewPix;
			++posTracker;
			++curPix;
		}

		//*img = im;
		++imgIn;
		++imgOut;
	}
	
	return true; // succes
}
