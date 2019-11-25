#ifndef MAXIMUMFILTER_H
#define MAXIMUMFILTER_H

#include "UnaryProcess.h"


class MaximumFilter : public UnaryProcess
{
public :
	MaximumFilter() = delete;
	MaximumFilter(int windowSize);
	~MaximumFilter() = default;


	void ProcessImage(std::vector<QImage> &image) override;

	void setWindowSize(int newWindowSize);
	size_t windowSize()const;

private:
	int mWindowSize = 1;

};

#endif // MAXIMUMFILTER