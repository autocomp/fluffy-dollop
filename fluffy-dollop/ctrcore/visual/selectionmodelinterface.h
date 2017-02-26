#ifndef SELECTIONMODELINTERFACE_H
#define SELECTIONMODELINTERFACE_H

#include "visualizerinterface.h"
#include <QSet>

namespace visualize_system
{

enum class DefaultRasterSelectionMode {COLOR, TRANSPARENTLY};

class SelectionModelInterface : public VisualizerInterface
{
    Q_OBJECT
public:
    explicit SelectionModelInterface(AbstractVisualizer& abstractVisualizer);
    
    QList<uint> getSelectedProviders();
    void setSelectProviders(bool on_off, const QList<uint> & providerIds);
    void reversSelection(const QList<uint> & providerIds);
    void clearSelectProviders();

    QList<uint> getMarkedProviders();
    void setMarkProvider(bool on_off, uint providerId);
    void setMarkProviders(bool on_off, const QList<uint> & providers);
    void clearMarkProviders();

    uint getCurrentProvider();
    void setCurrentProvider(uint providerId);
    void clearCurrentProvider();

    void setDefaultRasterSelectionMode(DefaultRasterSelectionMode mode);
    DefaultRasterSelectionMode getDefaultRasterSelectionMode();

signals:
    /**
     * @brief signalSelectedProvidersChanged - уведомление об изменении селекции провайдеров
     * @param providerSelected - все в данный момент селектированные провайдеры
     */
    void signalSelectedProvidersChanged(const QList<uint> & providerSelected);

    /**
     * @brief signalSelectedProvidersChanged - уведомление об изменении селекции провайдеров
     * @param providerSelected - провайдеры селектированные c момента предъидущей отправки сигнала
     * @param providerDeselected - провайдеры деселектированные c момента предъидущей отправки сигнала
     */
    void signalSelectedProvidersChanged(const QList<uint> & providerSelected, const QList<uint> & providerDeselected);

    /**
     * @brief signalMarkedProvidersChanged - уведомление об изменении отметки провайдеров
     * @param providerMarked - провайдеры отмеченные c момента предъидущей отправки сигнала
     * @param providerDemarked - провайдеры с которых снята отметка c момента предъидущей отправки сигнала
     */
    void signalMarkedProvidersChanged(const QList<uint> & providerMarked, const QList<uint> & providerDemarked);

    void signalCurrentProviderChanged(uint current, uint previous);

    void signalDefaultRasterSelectionModeChanged(visualize_system::DefaultRasterSelectionMode);

protected slots:
    void slotProvidersRemoved(const QList<uint> &);

protected:
    DefaultRasterSelectionMode _defaultRasterSelectionMode;
    QSet<uint> _selectProviders;
    QSet<uint> _markProviders;
    uint _currentProvider;
};
}
#endif // SELECTIONMODELINTERFACE_H
