#ifndef __ServerRequests_h_included__
#define __ServerRequests_h_included__

//REQUEST_HANDLER_DECL(SetPinValue, SET_PIN_VALUE, NULL);

class SetPinValue : public INetCommand
{
	REQUEST_HANDLER_BODY(SetPinValue, SET_PIN_VALUE, NULL);

	SetPinValue(string pinName, byte pinValue)
		: m_pinName(pinName)
		, m_pinValue(pinValue)
	{
	}

	virtual uint OnFillData(void * buffer, uint maxByteCount);

private:
	string m_pinName;
	byte m_pinValue;
};


#endif // #ifndef __ServerRequests_h_included__