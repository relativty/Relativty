#pragma once

#ifndef RELATIVTY_COMPONENTS_H
#define RELATIVTY_COMPONENTS_H

namespace Relativty {
  static const char *const k_pch_ExtDisplay_Section = "Relativty_extendedDisplay";
  static const char *const k_pch_ExtDisplay_WindowX_Int32 = "windowX";
  static const char *const k_pch_ExtDisplay_WindowY_Int32 = "windowY";
  static const char *const k_pch_ExtDisplay_WindowWidth_Int32 = "windowWidth";
  static const char *const k_pch_ExtDisplay_WindowHeight_Int32 = "windowHeight";
  static const char *const k_pch_ExtDisplay_RenderWidth_Int32 = "renderWidth";
  static const char *const k_pch_ExtDisplay_RenderHeight_Int32 = "renderHeight";
  static const char *const k_pch_ExtDisplay_DistortionK1_Float = "DistortionK1";
  static const char *const k_pch_ExtDisplay_DistortionK2_Float = "DistortionK2";
  static const char *const k_pch_ExtDisplay_ZoomWidth_Float = "ZoomWidth";
  static const char *const k_pch_ExtDisplay_ZoomHeight_Float = "ZoomHeight";
  static const char *const k_pch_ExtDisplay_EyeGapOffset_Int = "EyeGapOffsetPx";
  static const char *const k_pch_ExtDisplay_IsDisplayReal_Bool = "IsDisplayRealDisplay";
  static const char *const k_pch_ExtDisplay_IsDisplayOnDesktop_bool = "IsDisplayOnDesktop";


  static const bool g_bRelativtyExtDisplayComp_doLensStuff = true;
  class RelativtyExtendedDisplayComponent: public vr::IVRDisplayComponent {
  public:
    RelativtyExtendedDisplayComponent(){

      m_nWindowX = vr::VRSettings()->GetInt32(k_pch_ExtDisplay_Section,
                                              k_pch_ExtDisplay_WindowX_Int32);

      m_nWindowY = vr::VRSettings()->GetInt32(k_pch_ExtDisplay_Section,
                                              k_pch_ExtDisplay_WindowY_Int32);

      m_nWindowWidth = vr::VRSettings()->GetInt32(k_pch_ExtDisplay_Section,
                                                  k_pch_ExtDisplay_WindowWidth_Int32);

      m_nWindowHeight = vr::VRSettings()->GetInt32(
          k_pch_ExtDisplay_Section, k_pch_ExtDisplay_WindowHeight_Int32);

      m_nRenderWidth = vr::VRSettings()->GetInt32(k_pch_ExtDisplay_Section,
                                                  k_pch_ExtDisplay_RenderWidth_Int32);

      m_nRenderHeight = vr::VRSettings()->GetInt32(
          k_pch_ExtDisplay_Section, k_pch_ExtDisplay_RenderHeight_Int32);

      m_fDistortionK1 = vr::VRSettings()->GetFloat(
          k_pch_ExtDisplay_Section, k_pch_ExtDisplay_DistortionK1_Float);

      m_fDistortionK2 = vr::VRSettings()->GetFloat(
          k_pch_ExtDisplay_Section, k_pch_ExtDisplay_DistortionK2_Float);

      m_fZoomWidth = vr::VRSettings()->GetFloat(k_pch_ExtDisplay_Section,
                                                k_pch_ExtDisplay_ZoomWidth_Float);

      m_fZoomHeight = vr::VRSettings()->GetFloat(k_pch_ExtDisplay_Section,
                                                 k_pch_ExtDisplay_ZoomHeight_Float);

      m_iEyeGapOff = vr::VRSettings()->GetFloat(k_pch_ExtDisplay_Section,
                                                 k_pch_ExtDisplay_EyeGapOffset_Int);

      m_bIsDisplayReal = vr::VRSettings()->GetBool(k_pch_ExtDisplay_Section,
                                                 k_pch_ExtDisplay_IsDisplayReal_Bool);

      m_bIsDisplayOnDesktop = vr::VRSettings()->GetBool(k_pch_ExtDisplay_Section,
                                                 k_pch_ExtDisplay_IsDisplayOnDesktop_bool);


      #ifdef DRIVERLOG_H
      DriverLog("Extended display component created\n");
      DriverLog("distortion koeffs: k1=%f, k2=%f\n", m_fDistortionK1, m_fDistortionK2);
      DriverLog("render target: %dx%d\n", m_nRenderWidth, m_nRenderHeight);
      DriverLog("window target: %dx%d\n", m_nWindowWidth, m_nWindowHeight);
      DriverLog("eye gap offset: %d", m_iEyeGapOff);
      DriverLog("is display real: %d", (int)m_bIsDisplayReal);
      DriverLog("is display on desktop: %d", (int)m_bIsDisplayOnDesktop);
      #else
      vr::VRDriverLog()->Log("Extended display component created\n");
      #endif

    }

    virtual void GetWindowBounds(int32_t *pnX, int32_t *pnY, uint32_t *pnWidth,
                                 uint32_t *pnHeight) {
      *pnX = m_nWindowX;
      *pnY = m_nWindowY;
      *pnWidth = m_nWindowWidth;
      *pnHeight = m_nWindowHeight;
    }

    virtual bool IsDisplayOnDesktop() { return m_bIsDisplayOnDesktop; }

    virtual bool IsDisplayRealDisplay() { return m_bIsDisplayReal; }

    virtual void GetRecommendedRenderTargetSize(uint32_t *pnWidth,
                                                uint32_t *pnHeight) {
      *pnWidth = m_nRenderWidth;
      *pnHeight = m_nRenderHeight;
    }

    virtual void GetEyeOutputViewport(vr::EVREye eEye, uint32_t *pnX, uint32_t *pnY,
                                      uint32_t *pnWidth, uint32_t *pnHeight) {
      *pnY = 0;
      *pnWidth = m_nWindowWidth / 2;
      *pnHeight = m_nWindowHeight;

      if (eEye == vr::Eye_Left) {
        *pnX = 0;
      } else {
        *pnX = m_nWindowWidth / 2 + m_iEyeGapOff;
      }
    }

    virtual void GetProjectionRaw(vr::EVREye eEye, float *pfLeft, float *pfRight,
                                  float *pfTop, float *pfBottom) {
      *pfLeft = -1.0;
      *pfRight = 1.0;
      *pfTop = -1.0;
      *pfBottom = 1.0;
    }

    virtual vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU,
                                                      float fV) {
      vr::DistortionCoordinates_t coordinates;

      if constexpr(g_bRelativtyExtDisplayComp_doLensStuff) {
        // Distortion for lens implementation from
        // https://github.com/HelenXR/openvr_survivor/blob/master/src/head_mount_display_device.cc
        float hX;
        float hY;
        double rr;
        double r2;
        double theta;

        rr = sqrt((fU - 0.5f) * (fU - 0.5f) + (fV - 0.5f) * (fV - 0.5f));
        r2 = rr * (1 + m_fDistortionK1 * (rr * rr) +
                   m_fDistortionK2 * (rr * rr * rr * rr));
        theta = atan2(fU - 0.5f, fV - 0.5f);
        hX = float(sin(theta) * r2) * m_fZoomWidth;
        hY = float(cos(theta) * r2) * m_fZoomHeight;

        coordinates.rfBlue[0] = hX + 0.5f;
        coordinates.rfBlue[1] = hY + 0.5f;
        coordinates.rfGreen[0] = hX + 0.5f;
        coordinates.rfGreen[1] = hY + 0.5f;
        coordinates.rfRed[0] = hX + 0.5f;
        coordinates.rfRed[1] = hY + 0.5f;
      } else {
        coordinates.rfBlue[0] = fU;
        coordinates.rfBlue[1] = fV;
        coordinates.rfGreen[0] = fU;
        coordinates.rfGreen[1] = fV;
        coordinates.rfRed[0] = fU;
        coordinates.rfRed[1] = fV;
      }

      return coordinates;
    }

    const char* GetComponentNameAndVersion() {return vr::IVRDisplayComponent_Version;}

  private:
    int32_t m_nWindowX;
    int32_t m_nWindowY;
    int32_t m_nWindowWidth;
    int32_t m_nWindowHeight;
    int32_t m_nRenderWidth;
    int32_t m_nRenderHeight;
    int32_t m_iEyeGapOff;

    bool m_bIsDisplayReal;
    bool m_bIsDisplayOnDesktop;


    float m_fDistortionK1;
    float m_fDistortionK2;
    float m_fZoomWidth;
    float m_fZoomHeight;
  };

}

#endif // RELATIVTY_COMPONENTS_H