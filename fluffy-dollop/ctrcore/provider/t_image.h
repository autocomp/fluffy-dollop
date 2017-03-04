#ifndef T_IMAGE_H
#define T_IMAGE_H

#include <stdint.h>
#include <string.h>

/********
 *
 *
 */

namespace image_types
{
//! Перечисляемый тип, определяющий тип данных.
enum DATA_TYPE
{
    //! данными является изображение с матрицей
    BYT = 0,
    //! данными является радиоголограмм
    RGG,
    //! данными являются данные ГСС
    ENVI,
    //! данными является видеопоток
    VIDEO,
    //! данными является карта
    MAP,
    //! Premier
    RLI
};

//! Перечисляемый тип, определяющий тип отсчета яркости (пикселя) на матрице изображения.
enum PIXEL_TYPE
{
    PT_RGB = 0,
    PT_UINT_8,
    PT_INT_8,
    PT_UINT_16,
    PT_INT_16,
    PT_UINT_32,
    PT_INT_32,
    PT_UINT_64,
    PT_INT_64,
    PT_FLOAT,
    PT_DOUBLE
};

//! Структура для хранения палитры изображения.
/*!
      Палитру можно задавать при передаче изображения в виде структуры TImage.
      Также можно заменять существующую палитру, передавая отдельно TPalette.
 */
struct TPalette
{
    //! Указатель на связанную с изображением палитру. Если палитры нет, равен нулю.
    uint32_t *ctab;
    //! Количество цветов в палитре. Если палитры нет, равно нулю.
    uint32_t cnum;

    //! Конструктор по умолчанию
    TPalette()
    {
        ctab = 0;
        cnum = 0;
    }

    //! Метод возвращает копию палитры
    TPalette getCopy() const
    {
        TPalette p;

        if(ctab && cnum > 0)
        {
            p.cnum = cnum;
            p.ctab = new uint32_t[cnum];
            memcpy(p.ctab, ctab, cnum * sizeof(uint32_t));
        }

        return p;
    }
};

//! Структура для передачи уровней выходной гистограммы
struct TImageLevels
{
    bool flagR;
    bool flagG;
    bool flagB;
    bool flagGray;
    int levelsR[3];
    int levelsG[3];
    int levelsB[3];
    int levelsGray[3];
};

//! Структура для передачи параметров гистограммы изображения в визуализатор
struct THisto
{
    bool use; // true - гистограмма включена, false - выключена
    bool colored; // true - изображение цветное, false - нет
    bool dontUpdateViewer; // true - если нужно отправить данные в визуализатор, но не нужно обновлять
                           // изображение прямо сейчас
    TImageLevels levels;

    THisto() : use(false), colored(false), dontUpdateViewer(false)
    {
        memset(&levels, 0, sizeof(TImageLevels));
        const int a[3] = {0, 127, 255};
        for(int i = 0; i < 3; i++)
        {
            levels.levelsR[i] = a[i];
            levels.levelsG[i] = a[i];
            levels.levelsB[i] = a[i];
            levels.levelsGray[i] = a[i];
        }
    }

    bool equals(const THisto &r) const
    {
        return use == r.use && colored == r.colored && !memcmp(&levels, &r.levels, sizeof(TImageLevels));
    }
};

//! Структура для передачи матрицы изображения.
struct TImage
{
    //! Ширина матрицы
    uint32_t w;
    //! Высота изображения
    uint32_t h;
    //! Количество бит на пиксель
    uint32_t depth;
    //! Указатель на массив изображения
    char *prt;

    //! Тип данных
    DATA_TYPE dataType;

    //! Тип отсчета яркости (пикселя)
    PIXEL_TYPE pixType;

    //! Палитра изображения
    TPalette pal;

    //! Конструктор по умолчанию
    TImage()
        : w(0), h(0), depth(0), prt(0), dataType(BYT), pixType(PT_RGB)
    {}

    //! Метод для установки произвольного массива данных
    void setData(char *data, uint32_t size, DATA_TYPE dt, PIXEL_TYPE pt)
    {
        prt = data;
        w = size;
        h = 1;
        depth = 8;
        dataType = dt;
        pixType = pt;
    }

    uint32_t bytesPerPixel() const
    {
        return depth / 8;
    }

    uint32_t imageSize() const
    {
        return w * h * (depth / 8);
    }

    bool isValid()
    {
        return prt != 0;
    }
};

class Image
{
private:
    TImage m_image;

    void clearIfNeeded(const TImage &img)
    {
        if(m_image.prt != img.prt)
        {
            delete[] m_image.prt;
        }

        if(m_image.pal.ctab != img.pal.ctab)
        {
            delete[] m_image.pal.ctab;
        }
    }

public:
    /**
     * Конструктор 1
     */
    Image()
    {};

    /**
     * Конструктор 2
     */
    Image(const TImage &img)
    {
        setTImageFullCopy(img);
    };

    /**
     * Конструктор копирования
     * @param im - объект класса, копия котрого выполняется
     * выполняется полная копия
     */
    Image(const Image &img)
    {
        setTImageFullCopy(img.getTImage());
    }

    /**
     * Деструктор класса. Освобождает память, связанную с изображением.
     */
    ~Image()
    {
        clear();
    };

    Image &operator=(const Image &img)
    {
        if(&img != this)
        {
            setTImageFullCopy(img.getTImage());
        }

        return *this;
    }

    Image &operator=(const TImage &img)
    {
        setTImageFullCopy(img);
        return *this;
    }

    /**
     * Функция очистки памяти, память удаляется, размер
     * изображения сбрасывается в ноль
     */
    void clear()
    {
        delete[] m_image.prt;
        delete[] m_image.pal.ctab;
        m_image = TImage();
    }

    void setTImage(const TImage &img)
    {
        clearIfNeeded(img);
        m_image = img;
    }

    const TImage &getTImage() const
    {
        return m_image;
    }

    void setTImageWithPaletteCopy(const TImage &img)
    {
        clearIfNeeded(img);
        m_image = img;
        m_image.pal = img.pal.getCopy();
    }

    TImage getTImageWithPaletteCopy() const
    {
        TImage img = m_image;
        img.pal = m_image.pal.getCopy();
        return img;
    }

    void setTImageFullCopy(const TImage &img)
    {
        if(!img.prt)
        {
            return;
        }

        if(m_image.imageSize() == img.imageSize() && m_image.prt != img.prt)
        {
            char *saveBits = m_image.prt;

            if(m_image.pal.ctab != img.pal.ctab)
            {
                delete[] m_image.pal.ctab;
            }

            m_image = img;
            m_image.prt = saveBits;
            m_image.pal = img.pal.getCopy();

            memcpy(m_image.prt, img.prt, img.imageSize());
        }
        else
        {
            clearIfNeeded(img);
            m_image = img;
            m_image.pal = img.pal.getCopy();

            uint32_t imgSize = img.imageSize();
            m_image.prt = new char[imgSize];
            memcpy(m_image.prt, img.prt, imgSize);
        }
    }

    char *bits(uint32_t index) const
    {
        return (m_image.prt + (index * m_image.bytesPerPixel()));
    }

    char *bits() const
    {
        return m_image.prt;
    }

    uint32_t width() const
    {
        return m_image.w;
    }

    uint32_t height() const
    {
        return m_image.h;
    }

    uint32_t depth() const
    {
        return m_image.depth;
    }

    uint32_t bytesPerPixel() const
    {
        return m_image.bytesPerPixel();
    }

    PIXEL_TYPE pixelType() const
    {
        return m_image.pixType;
    }

    DATA_TYPE dataType() const
    {
        return m_image.dataType;
    }

    uint32_t imageSize() const
    {
        return m_image.imageSize();
    }

    const TPalette &palette() const
    {
        return m_image.pal;
    }

    void setPalette(const TPalette &p)
    {
        if(m_image.pal.ctab != p.ctab)
        {
            delete[] m_image.pal.ctab;
        }

        m_image.pal.ctab = 0;
        m_image.pal.cnum = 0;

        if(p.ctab && p.cnum > 0)
        {
            m_image.pal = p;
        }
    }

    bool isSet()
    {
        return m_image.prt;
    }

    void reset()
    {
        delete[] m_image.pal.ctab;
        m_image = TImage();
    }
};
}

#endif // ifndef T_IMAGE_H
