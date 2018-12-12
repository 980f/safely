

  const unsigned I2C_ADDRESS = 0x3C;    //# 011110+SA0+RW - 0x3C or 0x3D
//  enum Cmd {
//       SETCOMPINS = 0xDA,
//      SETLOWCOLUMN = 0x00,
//      SETHIGHCOLUMN = 0x10,

//      COLUMNADDR = 0x21,
//      PAGEADDR = 0x22,



/////# Scrolling constants
//      SET_VERTICAL_SCROLL_AREA = 0xA3,
//      RIGHT_HORIZONTAL_SCROLL = 0x26,
//      LEFT_HORIZONTAL_SCROLL = 0x27,
//      VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29,
//      VERTICAL_AND_LEFT_HORIZONTAL_SCROLL = 0x2A,

//};

#include "i2c.h"
#include "dout.h" //reset pin

class SSD1306{
//    """Base class for SSD1306-based OLED displays.  Implementors should subclass
//    and provide an implementation for the _initialize function.
//    """
    enum VCCmode{
        EXTERNAL = 1,
        SWITCHCAP = 2,
    };


public:
    unsigned width;
    unsigned height;

private:
    Dout resetpin;
    I2C dev;
    unsigned pages;
    uint8_t &buffer;
    VCCmode vccmode=0;

    //handles bits in opcode and 8 bits following
    template <unsigned code,unsigned bits=1,unsigned bytes=1,unsigned arf=0> class Register {
        enum {
            mask=((1<<bits)-1), //ones where operand bits are allowed
            aligner=8*bytes+arf, //little endian machine, must nominally reverse byte order
        };

        void operator =(unsigned value){
            unsigned pattern= (code<<8) | ((value &~mask)<<aligner);//single byte command
            dev.write(&pattern,bytes+1);//+1 for the C0/D byte
        }

    };

public:

    Register<0xA4> allOn;//1= all pixels lit
    Register<0xA6> inverseVideo; //inverts data on way ito the video buffer, doesn't alter existing image
    Register<0xAE> display;//1= show data
    Register<0x81,8,2> contrast;
    Register<0xD5,8,2> osc;
    Register<0xD9,8,2> precharge;
    Register<0xDB,3,2,4> vcomh;
    Register<0xA8,6,2> muxratio;//minimum of 15 not enforced resets to all ones==63
    Register<0xD3,6,2> displayOffset;
    Register<0x40,6,1> startLine;
    Register<0x2E> scrolling; //+1 to enable else off
    Register<0xA0> hflip;//      SEGREMAP = 0xA0,
    Register<0xC0,4,1,3> vflip;
    Register<0x2c0,2,2,4> compins;
//
    Register<0x20,8,2> memoryMode;//      MEMORYMODE = 0x20,
    Register<0x8D,0,0> chargePump;



    SSD1306(unsigned width, unsigned height, unsigned rst, unsigned dc=None, unsigned i2c_bus=1, unsigned i2c_address=I2C_ADDRESS):
        width (width),
        height(height),
        resetpin(rst), //will need to remap value somewhere
        dev(i2c_bus,i2c_address),
        pages (height/8),
        buffer (new uint8_t[width*pages]){
    }

    bool connect(){
        return dev.connect();
    }

    /** write one byte of display data */
    bool data(unsigned pixchunk){
        unsigned pattern=0x40 | (pixchunk<<8) ;
        dev.write(&pattern,2);//+1 for the C0/D byte
    }

    bool begin(unsigned vccstate=SWITCHCAPVCC){
        //        """Initialize display."""
        this->vccstate = vccstate;
//        # Reset and initialize display.
        reset();
        display=0;
        osc=0x80;
        muxratio=height-1;//number of lines -1 it do appear.
        displayOffset=0;
        startLine= 0;
        //looks funky:
        command(0x8D); command(vccmode==EXTERNAL?0x10:0x14);
        memoryMode=0;

        hflip=1;
        vflip=1;
        compins=height>=64?1:0;//0..3
        contrast=height>=64?(vccmode==EXTERNAL?0x9F:0xCF):0x8F;//?anal excretion

        precharge=vccmode==EXTERNAL?0x22:0xF1 ;//is actually two nibbles and neither should be 0
        vcomh=4;//manual offers 0,2,3 as
        initialize();
        display=1;
    }

    void initialize(){
        def _initialize(self):

    }

    void reset(){
//        """Reset the display."""
//        # Set reset high for a millisecond.
        resetpin=1;
        //do this via callbacks!
//+        MicroSecotime.sleep(0.001)
//        # Set reset low for 10 milliseconds.
        resetpin=0;
//+        time.sleep(0.010)
//        # Set reset high again.
        resetpin=1;
    }

    void refresh(){
//        """Write display buffer to physical display."""
        command(COLUMNADDR);
        command(0)      ;//# Column start address. (0 = reset)
        command(width-1);//   # Column end address.
        command(PAGEADDR);
        command(0);//              # Page start address. (0 = reset)
        command(pages-1);//  # Page end address.
//        # Write buffer data.
        for(unsigned i=0;i<sizeof(buffer); i+=16){//16 at a time
            dev.write(buffer+i,16);//todo: prefix with 0x40
        }

//    def image(self, image):
//        """Set buffer to value of Python Imaging Library image.  The image should
//        be in 1 bit mode and a size equal to the display size.
//        """
//        if image.mode != '1':
//            raise ValueError('Image must be in mode 1.')
//        imwidth, imheight = image.size
//        if imwidth != self.width or imheight != self.height:
//            raise ValueError('Image must be same dimensions as display ({0}x{1}).' \
//                .format(self.width, self.height))
//        # Grab all the pixels from the image, faster than getpixel.
//        pix = image.load()
//        # Iterate through the memory pages
//        index = 0
//        for page in range(self._pages):
//            # Iterate through all x axis columns.
//            for x in range(self.width):
//                # Set the bits for the column of pixels at the current position.
//                bits = 0
//                # Don't use range here as it's a bit slow
//                for bit in [0, 1, 2, 3, 4, 5, 6, 7]:
//                    bits = bits << 1
//                    bits |= 0 if pix[(x, page*8+7-bit)] == 0 else 1
//                # Update buffer byte and increment to next byte.
//                self._buffer[index] = bits
//                index += 1

//    def clear(self):
//        """Clear contents of image buffer."""
//        self._buffer = [0]*(self.width*self._pages)


    def dim(self, dim):
        """Adjusts contrast to dim the display if dim is True, otherwise sets the
        contrast to normal brightness if dim is False.
        """
        # Assume dim display.
        contrast = 0
        # Adjust contrast based on VCC if not dimming.
        if not dim:
            if self._vccstate == SSD1306_EXTERNALVCC:
                contrast = 0x9F
            else:
                contrast = 0xCF


class SSD1306_128_64(SSD1306Base):
    def __init__(self, rst, dc=None, sclk=None, din=None, cs=None, gpio=None,
                 spi=None, i2c_bus=None, i2c_address=SSD1306_I2C_ADDRESS,
                 i2c=None):
        # Call base class constructor.
        super(SSD1306_128_64, self).__init__(128, 64, rst, dc, sclk, din, cs,
                                             gpio, spi, i2c_bus, i2c_address, i2c)

    def _initialize(self):
        # 128x64 pixel specific initialization.
        self.command(SSD1306_DISPLAYOFF)                    # 0xAE
        self.command(SSD1306_SETDISPLAYCLOCKDIV)            # 0xD5
        self.command(0x80)                                  # the suggested ratio 0x80
        self.command(SSD1306_SETMULTIPLEX)                  # 0xA8
        self.command(0x3F)
        self.command(SSD1306_SETDISPLAYOFFSET)              # 0xD3
        self.command(0x0)                                   # no offset
        self.command(SSD1306_SETSTARTLINE | 0x0)            # line #0
        self.command(SSD1306_CHARGEPUMP)                    # 0x8D
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.command(0x10)
        else:
            self.command(0x14)

        self.command(SSD1306_MEMORYMODE)                    # 0x20
        self.command(0x00)                                  # 0x0 act like ks0108
        self.command(SSD1306_SEGREMAP | 0x1)
        self.command(SSD1306_COMSCANDEC)
        self.command(SSD1306_SETCOMPINS)                    # 0xDA
        self.command(0x12)
        self.command(SSD1306_SETCONTRAST)                   # 0x81
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.command(0x9F)
        else:
            self.command(0xCF)
        self.command(SSD1306_SETPRECHARGE)                  # 0xd9
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.command(0x22)
        else:
            self.command(0xF1)
        self.command(SSD1306_SETVCOMDETECT)                 # 0xDB
        self.command(0x40)
        self.command(SSD1306_DISPLAYALLON_RESUME)           # 0xA4
        self.command(SSD1306_NORMALDISPLAY)                 # 0xA6


class SSD1306_128_32(SSD1306Base):
    def __init__(self, rst, dc=None, sclk=None, din=None, cs=None, gpio=None,
                 spi=None, i2c_bus=None, i2c_address=SSD1306_I2C_ADDRESS,
                 i2c=None):
        # Call base class constructor.
        super(SSD1306_128_32, self).__init__(128, 32, rst, dc, sclk, din, cs,
                                             gpio, spi, i2c_bus, i2c_address, i2c)

    def _initialize(self):
        # 128x32 pixel specific initialization.
        self.command(SSD1306_DISPLAYOFF)                    # 0xAE
        self.command(SSD1306_SETDISPLAYCLOCKDIV)            # 0xD5
        self.command(0x80)                                  # the suggested ratio 0x80
        self.command(SSD1306_SETMULTIPLEX)                  # 0xA8
        self.command(0x1F)
        self.command(SSD1306_SETDISPLAYOFFSET)              # 0xD3
        self.command(0x0)                                   # no offset
        self.command(SSD1306_SETSTARTLINE | 0x0)            # line #0
        self.command(SSD1306_CHARGEPUMP)                    # 0x8D
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.command(0x10)
        else:
            self.command(0x14)
        self.command(SSD1306_MEMORYMODE)                    # 0x20
        self.command(0x00)                                  # 0x0 act like ks0108
        self.command(SSD1306_SEGREMAP | 0x1)
        self.command(SSD1306_COMSCANDEC)
        self.command(SSD1306_SETCOMPINS)                    # 0xDA
        self.command(0x02)
        self.command(SSD1306_SETCONTRAST)                   # 0x81
        self.command(0x8F)
        self.command(SSD1306_SETPRECHARGE)                  # 0xd9
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.command(0x22)
        else:
            self.command(0xF1)
        self.command(SSD1306_SETVCOMDETECT)                 # 0xDB
        self.command(0x40)
        self.command(SSD1306_DISPLAYALLON_RESUME)           # 0xA4
        self.command(SSD1306_NORMALDISPLAY)                 # 0xA6


class SSD1306_96_16(SSD1306Base):
    def __init__(self, rst, dc=None, sclk=None, din=None, cs=None, gpio=None,
                 spi=None, i2c_bus=None, i2c_address=SSD1306_I2C_ADDRESS,
                 i2c=None):
        # Call base class constructor.
        super(SSD1306_96_16, self).__init__(96, 16, rst, dc, sclk, din, cs,
                                            gpio, spi, i2c_bus, i2c_address, i2c)

    def _initialize(self):
        # 128x32 pixel specific initialization.
        self.command(SSD1306_DISPLAYOFF)                    # 0xAE
        self.command(SSD1306_SETDISPLAYCLOCKDIV)            # 0xD5
        self.command(0x60)                                  # the suggested ratio 0x60
        self.command(SSD1306_SETMULTIPLEX)                  # 0xA8
        self.command(0x0F)
        self.command(SSD1306_SETDISPLAYOFFSET)              # 0xD3
        self.command(0x0)                                   # no offset
        self.command(SSD1306_SETSTARTLINE | 0x0)            # line #0
        self.command(SSD1306_CHARGEPUMP)                    # 0x8D
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.command(0x10)
        else:
            self.command(0x14)
        self.command(SSD1306_MEMORYMODE)                    # 0x20
        self.command(0x00)                                  # 0x0 act like ks0108
        self.command(SSD1306_SEGREMAP | 0x1)
        self.command(SSD1306_COMSCANDEC)
        self.command(SSD1306_SETCOMPINS)                    # 0xDA
        self.command(0x02)
        self.command(SSD1306_SETCONTRAST)                   # 0x81
        self.command(0x8F)
        self.command(SSD1306_SETPRECHARGE)                  # 0xd9
        if self._vccstate == SSD1306_EXTERNALVCC:
            self.command(0x22)
        else:
            self.command(0xF1)
        self.command(SSD1306_SETVCOMDETECT)                 # 0xDB
        self.command(0x40)
        self.command(SSD1306_DISPLAYALLON_RESUME)           # 0xA4
        self.command(SSD1306_NORMALDISPLAY)                 # 0xA6
