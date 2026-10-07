#include <mxl/mxl.h>
#include <mxl/flow.h>
#include <mxl/time.h>
#include <cstring>
#include <cstdio>

#define SUCCESS 0
#define ERROR 1


int print_status(int status, const char *message) {
  if (status == SUCCESS) {
    system("Color 0A");
    std::fprintf(stdout, message);
    system("Color 0F");
  }
  else {
    system("Color 0C");
    std::fprintf(stderr, message);
    system("Color 0F");
  }
  return status;
}

int main( int argc, char *argv[])
{
  const char *mxlDomain = "/dev/shm/mxl";

  int i = 1;
  while (argv[i]) {
    if (std::strcmp(argv[i], "--mxl-domain") == 0) {
      if (i + 1 >= argc) {
        return print_status(ERROR, "ERROR: mxl-domain missing !");
      }
      mxlDomain = argv[++i];
    }
    else {
      return print_status(ERROR, "ERROR: please add --mxl-domain PATH ");
    }
  }

  mxlInstance instance = mxlCreateInstance(mxlDomain, "");
    if (!instance)
    {
        return print_status(ERROR, "ERROR: Instance missing");
    }
    mxlFlowWriter writer{};
    mxlFlowConfigInfo config{};
    bool created = false;

    const char *flowDefinition = R"json(
{
  "description": "ST 2110 receiver video output",
  "id": "5fbec3b1-1b0f-417d-9059-8b94a47197ed",
  "tags": {
    "urn:x-nmos:tag:grouphint/v1.0": [
      "My Media Function:Video"
    ]
  },
  "format": "urn:x-nmos:format:video",
  "label": "ST 2110 Video Output",
  "parents": [],
  "media_type": "video/v210",
  "grain_rate": {
    "numerator": 30000,
    "denominator": 1001
  },
  "frame_width": 1920,
  "frame_height": 1080,
  "interlace_mode": "progressive",
  "colorspace": "BT709",
  "components": [
    {
      "name": "Y",
      "width": 1920,
      "height": 1080,
      "bit_depth": 10
    },
    {
      "name": "Cb",
      "width": 960,
      "height": 1080,
      "bit_depth": 10
    },
    {
      "name": "Cr",
      "width": 960,
      "height": 1080,
      "bit_depth": 10
    }
  ]
}
)json";

    const mxlStatus status = mxlCreateFlowWriter(
        instance, flowDefinition, "", &writer, &config, &created);

    if (status != MXL_STATUS_OK)
    {
        mxlDestroyInstance(instance);
        return print_status(ERROR, "ERROR: MXL Flow couldn't be created");
    }

    const mxlRational frameRate{30000, 1001};
    const uint64_t frameIndex = mxlGetCurrentIndex(&frameRate);

    mxlGrainInfo grain{};
    uint8_t *payload = nullptr;

    const mxlStatus openStatus = mxlFlowWriterOpenGrain(
        writer, frameIndex, &grain, &payload
    );

    if (openStatus != MXL_STATUS_OK) {
        mxlReleaseFlowWriter(instance, writer);
        mxlDestroyInstance(instance);
        return print_status(ERROR, "ERROR: MXL grain couldn't be opened");
    }

    std::memset(payload, 0, grain.grainSize);

    const mxlStatus commitStatus = mxlFlowWriterCommitGrain(
        writer, &grain);

    if (commitStatus != MXL_STATUS_OK) {
        mxlReleaseFlowWriter(instance, writer);
        mxlDestroyInstance(instance);
        return print_status(ERROR, "ERROR: MXL grain couldn't be commited");
    }

    mxlReleaseFlowWriter(instance, writer);
    mxlDestroyInstance(instance);
    return print_status(SUCCESS, "SUCCESS: grain was written to domain");
}