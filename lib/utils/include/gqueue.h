//#ifndef __HGQUEUE_H
//#define __HGQUEUE_H

//#ifdef __cplusplus

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

	int push(II i);
	int pop(II *i);
	int operator << (II i);
	int operator >> (II&i);

    // return NULL on error
    II* watch(int i) const;
    II* prev() const;
    II* next() const;

    int write(const II *buf, uint32_t len);
	int read(II *buf, uint32_t blen);

	void set_lock_function(void (*f)(int));
	operator queue_s<II> *(); // dynamic_cast<Student *>(Person[i]);

};

template<typename II>
gqueue<II>::gqueue() 
{
    this->_flag = 0;
    this->_num = 0;
    this->_cp = this->_np = 0;
    this->_buf = 0;
}

template<typename II>
gqueue<II>::gqueue(int dnum, int mode) 
{
    this->_flag = 0;
    this->_discard = (mode == eQUEUE_DC_WAIT || mode == eQUEUE_DC_OLD)? mode : eQUEUE_DC_NEW;
    this->_num = dnum;
    this->_cp = this->_np = 0;
    this->_buf = new II[dnum];
    this->_alloc = 1;
}

template<typename II>
gqueue<II>::gqueue(II *buf, int dnum, int mode) 
{
    this->_flag = 0;
    this->_discard = (mode == eQUEUE_DC_WAIT || mode == eQUEUE_DC_OLD)? mode : eQUEUE_DC_NEW;

    this->_num = dnum;
    this->_cp = this->_np = 0;

    this->_buf = buf;
    this->_alloc = 0;
}

template<typename II>
gqueue<II>::~gqueue() 
{
    if( this->_alloc && this->_buf ) {
        delete[] this->_buf;
        this->_buf = 0;
        this->_alloc = 0;
    }
}

template<typename II>
int gqueue<II>::isfull() 
{
	return (((this->_cp + 1) == this->_np) || (this->_np == 0 && (this->_cp == (this->_num - 1))) );
}

template<typename II>
int gqueue<II>::isempty() 
{
	return (this->_cp == this->_np);
}

template<typename II>
int gqueue<II>::getfree()
{
	if( !isvalid() ) return -1;

	if( this->_cp <= this->_np ) {
		return (int)(this->_np - this->_cp);
	} else {
		return (int)(this->_num - this->_cp + this->_np);
	}
}

template<typename II>
int gqueue<II>::isvalid() 
{
    return ( this->_buf && this->_cp < this->_num && this->_np < this->_num )? 1 : 0;
}

template<typename II>
int gqueue<II>::init(int dnum, int mode) 
{
    this->_flag = 0;
    this->_discard = (mode == eQUEUE_DC_WAIT || mode == eQUEUE_DC_OLD)? mode : eQUEUE_DC_NEW;

    this->_num = dnum;
    this->_cp = this->_np = 0;

    this->_buf = new II[dnum];
    this->_alloc = 1;

    return 0;
}

template<typename II>
int gqueue<II>::clear() 
{
	if( !isvalid() )
		return -1;
	this->_cp = this->_np;

	return 0;
}

template<typename II>
int gqueue<II>::length() const 
{
	if( !isvalid() )
		return -1;

    return (this->_cp <= this->_np)? (this->_np - this->_cp) 
        : (this->_np + this->_num - this->_cp);
}

template<typename II>
int gqueue<II>::lock() 
{
    //__disable_irq();
    if( this->_lock == 1 ) {
        return -1;
    }
    this->_lock = 1;
    return this->_lock;
}

template<typename II>
int gqueue<II>::unlock() 
{
    //__enable_irq();
    if( this->_lock == 0 ) {
        return -1;
    }
    this->_lock = 0;
    return this->_lock;
}

template<typename II>
int gqueue<II>::push(II item) 
{
    if( !isvalid() ) {
        return -1;
    }

    lock();

    int re = 0;

	if( isfull() ) {
		this->_overflow = 1;
    
		if( this->_discard == eQUEUE_DC_OLD ) {
            this->_buf[this->_cp] = item;
            if( ++this->_cp >= this->_num ) this->_cp = 0;
            if( ++this->_np >= this->_num ) this->_np = 0; // discard old data
            re = 1;
        }
        // eQUEUE_DC_WAIT not proc...
	} else {
		this->_buf[this->_cp] = item;
		if( ++this->_cp >= this->_num ) this->_cp = 0;
        re = 1;
	}

    unlock();
   
	return re;
}

template<typename II>
int gqueue<II>::pop(II *ii) 
{
    if( !isvalid() ) {
        return -1;
    }

    int re = 0;

    lock();
	if( ! isempty() ) {
		*ii = this->_buf[this->_np];
		if( ++(this->_np) >= this->_num ) this->_np = 0;
        this->_overflow = 0;
        re = 1;
	} 
    unlock();

    return re;
}


template<typename II>
int gqueue<II>::operator << (II item) 
{
    if( !isvalid() ) {
        return -1;
    }

    lock();

    int re = 0;

	if( isfull() ) {
		this->_overflow = 1;
    
		if( this->_discard == eQUEUE_DC_OLD ) {
            this->_buf[this->_cp] = item;
            if( ++this->_cp >= this->_num ) this->_cp = 0;
            if( ++this->_np >= this->_num ) this->_np = 0; // discard old data
            re = 1;
        }
        // eQUEUE_DC_WAIT not proc...
	} else {
		this->_buf[this->_cp] = item;
		if( ++this->_cp >= this->_num ) this->_cp = 0;
        re = 1;
	}

    unlock();
   
	return re;
}

template<typename II>
int gqueue<II>::operator >> (II &ii) 
{
    if( !isvalid() ) {
        return -1;
    }

    int re = 0;

    lock();
	if( ! isempty() ) {
		ii = this->_buf[this->_np];
		if( ++(this->_np) >= this->_num ) this->_np = 0;
        this->_overflow = 0;
        re = 1;
	} 
    unlock();

    return re;
}

template<typename II>
int gqueue<II>::write(const II *buf, uint32_t len) 
{
    if( !isvalid() ) {
        return -1;
    }

    for( uint32_t u=0; u < len; u++ ) {
        if( push(buf[u]) != 1 ) {
            return (int)u;
        }
    }
    return len;
}

template<typename II>
int gqueue<II>::read(II *buf, uint32_t len) 
{
    if( !isvalid() ) {
        return -1;
    }

    for( uint32_t u=0; u < len; u++ ) {
        if( pop(buf[u]) != 1 ) {
            return (int)u;
        }
    }

    // if(len > 0x7fffffff)
    return (int)len;
}

template<typename II>
gqueue<II>::operator queue_s<II> *() 
{
    return (queue_s<II> *)*this;
}

template<typename II>
II* gqueue<II>::watch(int i) const 
{
    if( !isvalid() ) { //|| isempty() ) {
        return (II*)0;
    }

    if( i == 0 ) {
        return &this->_buf[this->_cp];
    }

    if( i > 0 ) { // --->
        if( i > (this->_num - this->_cp) ) {
            i -= (this->_num - this->_cp); 
        } else {
            i += this->_cp;
        }
    } else { // <---
        i = -i;
        if( i < this->_cp ) {
            i = this->_cp - i;
        } else {
            i = this->cp + this->_num - i;
        }
    }
    return &this->_buf[i];
}

template<typename II>
II* gqueue<II>::prev() const 
{
    return watch(-1);
}

template<typename II>
II* gqueue<II>::next() const 
{
    return watch(1);
}

//#endif // __cplusplus
//#endif // __HGQUEUE_H