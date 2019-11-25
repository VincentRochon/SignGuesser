#ifndef UNARYPROCESS_H
#define UNARYPROCESS_H

#include <QImage>



class UnaryProcess {
public:
	UnaryProcess() = default;
	virtual ~UnaryProcess() = default;

	virtual void ProcessImage(std::vector<QImage> &image) =0;

};

#endif // UNARYPROCESS_H