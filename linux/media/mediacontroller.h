#ifndef MEDIACONTROLLER_H
#define MEDIACONTROLLER_H

#include <QObject>
#include "pulseaudiocontroller.h"

class QProcess;
class EarDetection;
class PlayerStatusWatcher;
class QDBusInterface;
class QTimer;

class MediaController : public QObject
{
  Q_OBJECT
public:
  enum MediaState
  {
    Playing,
    Paused,
    Stopped
  };
  Q_ENUM(MediaState)
  enum EarDetectionBehavior
  {
    PauseWhenOneRemoved,
    PauseWhenBothRemoved,
    Disabled
  };
  Q_ENUM(EarDetectionBehavior)

  explicit MediaController(QObject *parent = nullptr);
  ~MediaController();

  void handleEarDetection(EarDetection*);
  void followMediaChanges();
  bool isActiveOutputDeviceAirPods();
  void handleConversationalAwareness(const QByteArray &data);
  void activateA2dpProfile();
  void removeAudioOutputDevice();
  // Route audio to the AirPods (A2DP) or away from them (profile off)
  void setAudioWanted(bool want);
  void setConnectedDeviceMacAddress(const QString &macAddress);
  bool isA2dpProfileAvailable();
  QString getPreferredA2dpProfile();
  bool restartWirePlumber();

  void setEarDetectionBehavior(EarDetectionBehavior behavior);
  inline EarDetectionBehavior getEarDetectionBehavior() const { return earDetectionBehavior; }

  void play();
  void pause();
  MediaState getCurrentMediaState() const;

Q_SIGNALS:
  void mediaStateChanged(MediaState state);

private:
  MediaState mediaStateFromPlayerctlOutput(const QString &output) const;
  QString getAudioDeviceName();
  void applyAudioRoute();
  QStringList getPlayingMediaPlayers();

  QStringList pausedByAppServices;
  int initialVolume = -1;
  QString connectedDeviceMacAddress;
  EarDetectionBehavior earDetectionBehavior = PauseWhenOneRemoved;
  QString m_deviceOutputName;
  PlayerStatusWatcher *playerStatusWatcher = nullptr;
  PulseAudioController *m_pulseAudio = nullptr;
  QString m_cachedA2dpProfile;
  // The bluez card shows up a moment after the AirPods connect, so the
  // ear-detection decision is retried until the card exists
  bool m_wantAudio = true;
  int m_cardRetries = 0;
  QTimer *m_cardRetryTimer = nullptr;
};

#endif // MEDIACONTROLLER_H