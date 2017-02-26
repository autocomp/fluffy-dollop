#include "selectionmodelinterface.h"
#include "datainterface.h"

using namespace visualize_system;

SelectionModelInterface::SelectionModelInterface(AbstractVisualizer& abstractVisualizer)
    : VisualizerInterface(abstractVisualizer)
    , _defaultRasterSelectionMode(DefaultRasterSelectionMode::COLOR)
    , _currentProvider(0)
{
    if(_abstractVisualizer.getDataInterface())
        connect(_abstractVisualizer.getDataInterface(), SIGNAL(signalProvidersRemoved(QList<uint>)), this, SLOT(slotProvidersRemoved(QList<uint>)));
}

QList<uint> SelectionModelInterface::getSelectedProviders()
{
    return _selectProviders.toList();
}

void SelectionModelInterface::setSelectProviders(bool on_off, const QList<uint> & providerIds)
{
    QList<uint> selectedproviders;
    QList<uint> deselectedproviders;
    if(on_off)
    {
        foreach(uint id, providerIds)
        {
            auto it = _selectProviders.find(id);
            if(it == _selectProviders.end())
            {
                _selectProviders.insert(id);
                selectedproviders.append(id);
            }
        }
    }
    else
    {
        foreach(uint id, providerIds)
        {
            auto it = _selectProviders.find(id);
            if(it != _selectProviders.end())
            {
                _selectProviders.erase(it);
                deselectedproviders.append(id);
            }
        }
    }

    if(selectedproviders.isEmpty() == false || deselectedproviders.isEmpty() == false)
    {
        emit signalSelectedProvidersChanged(selectedproviders, deselectedproviders);
        emit signalSelectedProvidersChanged(_selectProviders.toList());
    }
}

void SelectionModelInterface::reversSelection(const QList<uint> & providerIds)
{
    QList<uint> selectedproviders;
    QList<uint> deselectedproviders;
    // если провайдер селектирован - он деселектируется !
    foreach(uint id, providerIds)
    {
        auto it = _selectProviders.find(id);
        if(it != _selectProviders.end())
        {
            _selectProviders.erase(it);
            deselectedproviders.append(id);
        }
        else
        {
            _selectProviders.insert(id);
            selectedproviders.append(id);
        }
    }
    emit signalSelectedProvidersChanged(selectedproviders, deselectedproviders);
    emit signalSelectedProvidersChanged(_selectProviders.toList());
}

void SelectionModelInterface::clearSelectProviders()
{
    QList<uint> deselectedproviders = _selectProviders.toList();
    _selectProviders.clear();
    QList<uint> emtySelectList;
    emit signalSelectedProvidersChanged(emtySelectList, deselectedproviders);
    emit signalSelectedProvidersChanged(_selectProviders.toList());
}

QList<uint> SelectionModelInterface::getMarkedProviders()
{
    return _markProviders.toList();
}

void SelectionModelInterface::setMarkProvider(bool on_off, uint providerId)
{
    QList<uint> markedList;
    QList<uint> demarkedList;

    if(on_off)
    {
        _markProviders.insert(providerId);
        markedList.append(providerId);
    }
    else
    {
        _markProviders.remove(providerId);
        demarkedList.append(providerId);
    }

    emit signalMarkedProvidersChanged(markedList, demarkedList);
}

void SelectionModelInterface::setMarkProviders(bool on_off, const QList<uint> & providers)
{
    QList<uint> markedList;
    QList<uint> demarkedList;

    if(on_off)
    {
        foreach(uint providerId, providers)
        {
            _markProviders.insert(providerId);
            markedList.append(providerId);
        }
    }
    else
    {
        foreach(uint providerId, providers)
        {
            _markProviders.remove(providerId);
            demarkedList.append(providerId);
        }
    }

    emit signalMarkedProvidersChanged(markedList, demarkedList);
}

void SelectionModelInterface::clearMarkProviders()
{
    QList<uint> demarkedproviders = _markProviders.toList();
    _markProviders.clear();
    QList<uint> emptyList;
    emit signalMarkedProvidersChanged(emptyList, demarkedproviders);
}

void SelectionModelInterface::slotProvidersRemoved(const QList<uint> & list)
{
    QList<uint> deselectedproviders, demarkedproviders;
    foreach(uint id, list)
    {
        if(_selectProviders.remove(id))
            deselectedproviders.append(id);
        if(_markProviders.remove(id))
            demarkedproviders.append(id);
    }
    if(deselectedproviders.isEmpty())
    {
        emit signalSelectedProvidersChanged(QList<uint>(), deselectedproviders);
        emit signalSelectedProvidersChanged(_selectProviders.toList());
    }
    if(demarkedproviders.isEmpty())
        emit signalMarkedProvidersChanged(QList<uint>(), demarkedproviders);
}

uint SelectionModelInterface::getCurrentProvider()
{
    return _currentProvider;
}

void SelectionModelInterface::setCurrentProvider(uint providerId)
{
    uint previous ( providerId == _currentProvider ? 0 : _currentProvider );
    _currentProvider = providerId;
    emit signalCurrentProviderChanged(_currentProvider, previous);
}

void SelectionModelInterface::clearCurrentProvider()
{
    uint previous = _currentProvider;
    _currentProvider = 0;
    emit signalCurrentProviderChanged(_currentProvider, previous);
}

void SelectionModelInterface::setDefaultRasterSelectionMode(DefaultRasterSelectionMode mode)
{
    _defaultRasterSelectionMode = mode;
    emit signalDefaultRasterSelectionModeChanged(_defaultRasterSelectionMode);
}

DefaultRasterSelectionMode SelectionModelInterface::getDefaultRasterSelectionMode()
{
    return _defaultRasterSelectionMode;
}





























