#include <mxl/mxl.h>
#include <mxl/flow.h>
#include <mxl/time.h>
#include <cstring>

#define SUCCESS 0
#define ERROR 1

int main()
{
    mxlInstance instance = mxlCreateInstance("/dev/shm/mxl", "");
    if (!instance)
    {
        return ERROR;
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
        return ERROR;
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
        return ERROR;
    }

    std::memset(payload, 0, grain.grainSize);

    const mxlStatus commitStatus = mxlFlowWriterCommitGrain(
        writer, &grain);

    if (commitStatus != MXL_STATUS_OK) {
        mxlReleaseFlowWriter(instance, writer);
        mxlDestroyInstance(instance);
        return ERROR;
    }

    mxlReleaseFlowWriter(instance, writer);
    mxlDestroyInstance(instance);
    return SUCCESS;
}