#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include <libpdjson/pdjson.h>

/* Parse the data in the specified stream mode returning true if the data is
   valid JSON and false otherwise. */
static bool
parse(const void *data, size_t size, bool stream)
{
  struct json_stream json[1];
  enum json_type t = JSON_DONE;

  json_open_buffer(json, data, size);
  json_set_streaming(json, stream);

  while (t != JSON_ERROR && (t = json_next(json)) != JSON_DONE)
  {
    /* Let's get a warning if any new values are added. */
    switch (t)
    {
    case JSON_DONE:   assert(false);               break;
    case JSON_ERROR:  json_get_error(json);        break;
    case JSON_STRING: json_get_string(json, NULL); break;
    case JSON_NUMBER: json_get_number(json);       break;
    case JSON_TRUE:
    case JSON_FALSE:
    case JSON_NULL:
    case JSON_OBJECT:
    case JSON_OBJECT_END:
    case JSON_ARRAY:
    case JSON_ARRAY_END: break;
    }
  }

  json_close(json);

  return t != JSON_ERROR;
}

int
LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  /* If it's valid in the strict mode, don't waste time parsing it in
     relaxed. */
  if (!parse(data, size, false))
    parse(data, size, true);
  return 0;
}
