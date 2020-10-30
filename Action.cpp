#include "Action.h"


Action::Action(Image* image, ACTION_TYPE type) {
	this->imgData = image->imgData;
	this->type = type;
	this->width = image->width;
	this->height = image->height;
	this->channels = image->channels;
	this->bitDepth = image->bitDepth;
	this->dpi = image->dpi;
	this->size = image->size;
	this->ext = image->ext;
	this->path = image->path;
	this->format = image->format;
	this->internalFormat = image->internalFormat;
}

Action::~Action() {

}
