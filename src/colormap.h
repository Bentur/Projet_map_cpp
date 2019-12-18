#ifndef __COLORMAP_H__
#define __COLORMAP_H__

class Colormap {

  public:
  	Colormap();
  	void getColor(float h, int& r, int& g, int& b);
  	void getGreyColor(float h, int& r, int& g, int& b);
  private:

};
#endif