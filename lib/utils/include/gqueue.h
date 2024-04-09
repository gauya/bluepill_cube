#ifndef __HGQUEUE_H
#define __HGQUEUE_H

#ifdef __cplusplus

enum { eQUEUE_DC_NEW, eQUEUE_DC_OLD, eQUEUE_DC_WAIT };

template<typename DT>
struct queue_s {
	union {
		uint16_t _flag;
        struct {
            uint16_t	_alloc    : 1;
            uint16_t	_lock     : 1;
            uint16_t	_overflow : 1;
            uint16_t	_discard  : 2;  // 0: eQUEUE_DC_NEW, eQUEUE_DC_OLD, eQUEUE_DC_WAIT
        };
	};
	uint32_t	_num;  // sizeof(DT) * _num
	uint32_t	_cp;		// write and move
	uint32_t	_np;		// not empty then read and move

	DT	*_buf;
};

template<typename II>
#if 1
class gqueue : public queue_s<II> {
#else    
class gqueue {
private:
	union {
		uint16_t _flag;
        struct {
            uint16_t	_alloc    : 1;
            uint16_t	_lock     : 1;
            uint16_t	_overflow : 1;
            uint16_t	_discard  : 2;  // 0: eQUEUE_DC_NEW, eQUEUE_DC_OLD, eQUEUE_DC_WAIT
        };
	};
	uint32_t	_num;  // sizeof(DT) * _num
	uint32_t	_cp;		// write and move
	uint32_t	_np;		// not empty then read and move

	II	*_buf;
#endif
protected:
	int isfull();
	int isempty();
	int isvalid();
    int getfree();
public:
	gqueue();
	gqueue(int dnum, int mode=0);
	gqueue(II *buf, int dnum, int mode=0); // use external buffer
	~gqueue();

	int init(int dnum, int mode=0);

	int clear(); // empty
	int length() const;
    int lock();
    int unlock();

	int push(II item);
	int pop(II *ii);
	
    // return NULL on error
    II* watch(int i) const;
    II* prev() const;
    II* next() const;

    int write(const II *buf, uint32_t len);
	int read(II *buf, uint32_t blen);

	void set_lock_function(void (*f)(int));
	operator queue_s<II> *(); // dynamic_cast<Student *>(Person[i]);
};

#endif // __cplusplus
#endif // __HGQUEUE_H