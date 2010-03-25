#ifndef TRACER_ERROR_H__
#define TRACER_ERROR_H__

class tracer_error : public std::exception
{
public:
	explicit tracer_error(const std::string& msg)
	 : m_msg(msg)
	{}

	virtual ~tracer_error() throw() // destroy the object
	{}

	virtual const char* what() const throw() // return pointer to message string
	{
		return m_msg.c_str();
	}

private:
	std::string m_msg;
};

#endif // TRACER_ERROR_H__