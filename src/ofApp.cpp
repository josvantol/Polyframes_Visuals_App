#include "ofApp.h"

#define SECONDS_TO_FADE 2.0f
#define SOUND_VOLUME 1.0f

static ofDirectory Folder;
static ofVideoPlayer VideoContainer;
static ofSoundPlayer TestSoundLeft;
static ofSoundPlayer TestSoundRight;

static bool Playing;
static bool ButtonDown;
static bool FadeOut;

static int VideoIndex;
static int FramesToFade;
static int FadeCounter;
static float Alpha;

typedef enum application_state
{
    Main,
    Playlist,
    Help
} application_state;

application_state State;

static void LoadVideo()
{
  VideoContainer.load(Folder.getPath(VideoIndex));
  VideoContainer.setVolume(SOUND_VOLUME);
  VideoContainer.setLoopState(OF_LOOP_NONE);
}

static void LoadNextVideo()
{
  if (VideoIndex == (Folder.size() - 1))
  {
    VideoIndex = 0;
  }
  else
  {
    VideoIndex++;
  }

  VideoContainer.load(Folder.getPath(VideoIndex));
  VideoContainer.setVolume(SOUND_VOLUME);
  VideoContainer.setLoopState(OF_LOOP_NONE);

  return;
}

void ofApp::setup()
{
  Playing = false;
  ButtonDown = false;
  FadeOut = false;
  VideoIndex = 0;
  FramesToFade = (int)(ofGetFrameRate() * SECONDS_TO_FADE);
  FadeCounter = 0;
  Alpha = 255.0f;

  Folder.open("");
  Folder.allowExt("mp4");
  Folder.listDir();
  if (Folder.size() == 0)
  {
    cout << "POLYFRAMES: No MP4-files found in \"data\" folder." << endl;
    cout << "POLYFRAMES: Exiting..." << endl;
    ofExit();
  }
  else
  {
    Folder.sort();
    LoadVideo();
  }

  State = Main;

  ofHideCursor();
}

void ofApp::update()
{
  FramesToFade = (int)(ofGetFrameRate() * SECONDS_TO_FADE);

  if (FadeOut)
  {
    Alpha = 255.0f - (float)FadeCounter * (255.0f / (float)FramesToFade);
    FadeCounter++;

    if (Alpha <= 0)
    {
      VideoContainer.stop();
      FadeOut = false;
      Playing = false;
      FadeCounter = 0;

      LoadNextVideo();
    }
  }

  VideoContainer.update();
}

static void DrawFrame()
{
  if (FadeOut)
  {
    ofSetColor(255, 255, 255, Alpha);
    VideoContainer.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  }
  else if (Playing)
  {
    ofSetColor(255, 255, 255, 255);
    VideoContainer.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  }
}

void ofApp::draw()
{
  ofBackground(0);

  switch (State)
  {
    case Main:
    {
      TestSoundLeft.stop();
      TestSoundRight.stop();
      DrawFrame();
      break;
    }

    case Help:
    {
      TestSoundLeft.stop();
      TestSoundRight.stop();
      DrawFrame();

      string HelpString = "POLYFRAMES Live Visuals App (F1)\n\n----------------------------\n\nESC: Close application\n\nF1: This help\nF2: Print playlist\n\nSpace: Start next video or fade out if already playing\n1 - 9: Start video in playlist\n\nF: Toggle fullscreen\n\nVersion: Oct 6, 2020";

      ofSetColor(255);
      ofDrawBitmapStringHighlight(HelpString, 100, 100);

      break;
    }

    case Playlist:
    {
      TestSoundLeft.stop();
      TestSoundRight.stop();
      DrawFrame();

      string Output = "TRACK LIST (F2)\n\n----------------------------\n\n";
      for (int TrackIndex = 0; TrackIndex < Folder.size(); TrackIndex++)
      {
        if (TrackIndex == VideoIndex)
        {
          Output += "> ";
        }
        else
        {
          Output += "  ";
        }
        Output += to_string(TrackIndex + 1) + ": " + Folder.getPath(TrackIndex) + "\n";
      }
      ofDrawBitmapStringHighlight(Output, 100, 100);

      break;
    }
  }
}

void ofApp::keyPressed(int key)
{
  if (ButtonDown == false)
  {
    if (key == ' ')
    {
      if (VideoContainer.getIsMovieDone())
      {
        LoadNextVideo();
        VideoContainer.play();
        Playing = true;
      }
      else if (Playing)
      {
        FadeOut = true;
      }
      else
      {
        VideoContainer.play();
        Playing = true;
      }

      ButtonDown = true;
    }

    if (key == 'f')
    {
      ofToggleFullscreen();
      ButtonDown = true;
    }

    if (key >= '1' && key <= '9')
    {
      if (key - '0' > Folder.size())
      {
        return;
      }

      VideoContainer.stop();
      FadeOut = false;
      Playing = false;
      FadeCounter = 0;

      VideoIndex = key - '1';
      LoadVideo();
      VideoContainer.play();
      Playing = true;

      ButtonDown = true;
    }

    if (key == OF_KEY_F1)
    {
      if (State == Help)
      {
        State = Main;
      }
      else
      {
        State = Help;
      }

      ButtonDown = true;
    }

    if (key == OF_KEY_F2)
    {
      if (State == Playlist)
      {
        State = Main;
      }
      else
      {
        State = Playlist;
      }

      ButtonDown = true;
    }
  }
}

void ofApp::keyReleased(int key)
{
  ButtonDown = false;
}
