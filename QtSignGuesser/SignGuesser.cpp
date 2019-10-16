#include "SignGuesser.h"

#include <QCameraInfo>


#include <QVBoxLayout>
#include "QSimpleImageGrabber.h"

SignGuesser::SignGuesser(QWidget *parent)
	: QMainWindow(parent),
	mConnectButton{ new QPushButton("Connect") },
	mDisconnectButton{ new QPushButton("Disconnect") },
	mCaptureOneButton{ new QPushButton("Capture one image") },
	mCaptureContinuouslyButton{ new QPushButton("Start capture continuously") },
	mInputImage{ new QSimpleImageViewer },
	mProcessedImage{ new QSimpleImageViewer },
	mCapturingContinuously{ false }
{
	ui.setupUi(this);

	QVBoxLayout* layout{ new QVBoxLayout };
	layout->addWidget(mConnectButton);
	layout->addWidget(mDisconnectButton);
	layout->addWidget(mCaptureOneButton);
	layout->addWidget(mCaptureContinuouslyButton);
	layout->addWidget(mInputImage);
	layout->addWidget(mProcessedImage);

	QWidget* centralWidget{ new QWidget };
	centralWidget->setLayout(layout);

	setCentralWidget(centralWidget);

	connect(mConnectButton, &QPushButton::clicked, this, &SignGuesser::connectCamera);
	connect(mDisconnectButton, &QPushButton::clicked, this, &SignGuesser::disconnectCamera);
	connect(mCaptureOneButton, &QPushButton::clicked, this, &SignGuesser::captureOne);
	connect(mCaptureContinuouslyButton, &QPushButton::clicked, this, &SignGuesser::captureContinuously);

	connect(&mSimpleImageGrabber, &QSimpleImageGrabber::imageCaptured, mInputImage, &QSimpleImageViewer::setImage);
	connect(&mSimpleImageGrabber, &QSimpleImageGrabber::imageCaptured, this, &SignGuesser::process01);
	//connect(&mSimpleImageGrabber, &QSimpleImageGrabber::imageCaptured, this, &ImgDemo::processCapturedImage);
	connect(&mSimpleImageGrabber, &QSimpleImageGrabber::readyForCaptureChanged, this, &SignGuesser::processReadyToCapture);

	updateGui();
}

void SignGuesser::connectCamera()
{
	mSimpleImageGrabber.connect();
	updateGui();
}

void SignGuesser::disconnectCamera()
{
	mSimpleImageGrabber.disconnect();
	updateGui();
}

void SignGuesser::captureOne()
{
	mSimpleImageGrabber.capture();
	updateGui();
}

void SignGuesser::captureContinuously()
{
	mCapturingContinuously = !mCapturingContinuously;
	mSimpleImageGrabber.capture();
	updateGui();
}

void SignGuesser::processReadyToCapture(bool ready)
{
	if (ready && mCapturingContinuously) {
		mSimpleImageGrabber.capture();
	}
}

void SignGuesser::updateGui()
{
	mConnectButton->setEnabled(!mSimpleImageGrabber.isConnected());
	mDisconnectButton->setEnabled(mSimpleImageGrabber.isConnected());
	mCaptureOneButton->setEnabled(mSimpleImageGrabber.isConnected() && !mCapturingContinuously);
	mCaptureContinuouslyButton->setEnabled(mSimpleImageGrabber.isConnected());
	mCaptureContinuouslyButton->setText(mCapturingContinuously ? "Stop capture continuously" : "Start capture continuously");
}

void SignGuesser::process01(QImage const& image)
{
	QImage::Format f{ image.format() };
	QImage im(image);

	// Scrap pour voir comment pas faire!!!
	//for (int x{}; x < im.width(); ++x) {
	//    for (int y{}; y < im.height(); ++y) {
	//        QColor color(im.pixel(x, y));
	//        color.setRed(255 - color.red());
	//        color.setGreen(255 - color.green());
	//        color.setBlue(255 - color.blue());
	//
	//        im.setPixelColor(x, y, color);
	//    }
	//}

	int* curPix{ reinterpret_cast<int*>(im.bits()) };
	int* endPix{ curPix + im.width() * im.height() };
	while (curPix < endPix) {
		int c{ *curPix };
		//*curPix =   ((255 - static_cast<unsigned char>((c & 0x00'FF'00'00) >> 16)) << 16) |
		//            ((255 - static_cast<unsigned char>((c & 0x00'00'FF'00) >>  8)) <<  8) |
		//            ((255 - static_cast<unsigned char>((c & 0x00'00'00'FF) >>  0)) <<  0) |
		//            0xFF'00'00'00;

		unsigned char r{ static_cast<unsigned char>((c & 0x00'FF'00'00) >> 16) };
		unsigned char g{ static_cast<unsigned char>((c & 0x00'00'FF'00) >> 8) };
		unsigned char b{ static_cast<unsigned char>((c & 0x00'00'00'FF) >> 0) };

		// moyenne g�om�trique
		//int average{ ((int)r + (int)g + (int)b) / 3 }; 
		// moyenne anthropomorphique
		int average{ static_cast<int>((float)r * 0.25f + (float)g * 0.67f + (float)b * 0.08f) };
		*curPix = (average << 16) |
			(average << 8) |
			(average << 0) |
			0xFF'00'00'00;

		++curPix;
	}

	mProcessedImage->setImage(im);

	//!emit imageProcessed(im);
}
