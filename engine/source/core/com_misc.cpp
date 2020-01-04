#include "core/common/com_misc.h"
#include "platform.h"

//////////////////////////////////////////////////////////////////////////
// Text Box

static pOnTextBoxResult* g_text_box_result = NULL;

void TextBox_SetHook(pOnTextBoxResult* phook)
{
  g_text_box_result = phook;
}

extern "C" void OnTextBoxResult(bool res)
{
  if(NULL != g_text_box_result)
    g_text_box_result(res);
}

TextBoxListener::TextBoxListener()
: m_buffer(NULL)
, m_own_size(0)
{}

TextBoxListener::~TextBoxListener()
{
  UnHook();
  DestroyBuffer();
}

void TextBoxListener::DestroyBuffer()
{
  if(0 != m_own_size)
    SAFE_DELETE_ARRAY(m_buffer);
  m_own_size = 0;
}

void TextBoxListener::TextBox_CallInPlace(const wchar_t * const szTitle,       wchar_t* buffer, unsigned int size)
{
  EASSERT(0 != size);
  DestroyBuffer();
  m_buffer = buffer;
  Hook();
  TextBox(szTitle, m_buffer, size);
}

void TextBoxListener::TextBox_CallAlloc  (const wchar_t * const szTitle, const wchar_t* buffer, unsigned int size)
{
  EASSERT(0 != size);
  if(size > m_own_size)
  {
    DestroyBuffer();
    m_buffer = new wchar_t[size];
    m_own_size = size;
  }
  // user has provided text
  if(buffer != NULL)
  {
    wchar_t* d = m_buffer;
    while(buffer)
      *d++ = *buffer++;
    d = NULL;
  }
  Hook();
  TextBox(szTitle, m_buffer, size);
}

TextBoxListener* TextBoxListener::g_listener = NULL;

void TextBoxListener::OnResult(bool res)
{
  TextBox_OnResult(res, m_buffer);
  UnHook();
}

void TextBoxListener::sOnResult(bool res)
{
  if(NULL != g_listener)
    g_listener->OnResult(res);
}

void TextBoxListener::UnHook()
{
  if(this == g_listener)
  {
    g_listener        = NULL;
    g_text_box_result = NULL;
  }
}

void TextBoxListener::Hook()
{
  EASSERT(NULL == g_text_box_result);
  EASSERT(NULL == g_listener);
  g_listener        = this;
  g_text_box_result = &TextBoxListener::sOnResult;
}
