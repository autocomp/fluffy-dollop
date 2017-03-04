#include "undocontroller.h"
#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/provider/rasterdataprovider.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>

using namespace undo_actions;

uint UndoAction::ID = 0;

UndoAction::UndoAction(const QString& text)
    : _id(++ID)
    , _text(text)
{
}

QString UndoAction::getText()
{
    return _text;
}

uint UndoAction::getId()
{
    return _id;
}

//----------------------------

RasterUndoAction::RasterUndoAction(const QString &_name, const QList<uint> &_provIds)
    : UndoAction(_name), provIds(_provIds.toSet())
{
}

bool RasterUndoAction::undo()
{
    bool ok(true);
    emit chechUndoMatchingPossible(ok);
    if(ok)
        foreach(uint id, provIds)
        {
            QSharedPointer<data_system::RasterDataProvider>rdp = data_system::DataProviderFactory::instance()->getProvider(id).dynamicCast<data_system::RasterDataProvider>();
            if(rdp.isNull() == false)
                rdp->undo();
        }
    return ok;
}

void RasterUndoAction::providerSavedOrDestroyed(uint rasterId)
{
    provIds.remove(rasterId);
}

bool RasterUndoAction::isValid()
{
    return provIds.isEmpty() == false;
}

//----------------------------

UndoActionInfo::UndoActionInfo(QString _text, uint _id)
    : text(_text), id(_id)
{}

//----------------------------

UndoController * UndoController::_instance = 0;

UndoController * UndoController::instance()
{
    if(!_instance)
        _instance = new UndoController();
    return _instance;
}

UndoController::UndoController()
    : _maxStackSize(100)
    , _controllerEnabled(true)
{
    QVariant maxStackSizeVar = CtrConfig::getValueByName(QString("application_settings.maxStackSize_elements"));
    if(maxStackSizeVar.isValid())
        _maxStackSize = maxStackSizeVar.toUInt();
    else
        CtrConfig::setValueByName(QString("application_settings.maxStackSize_elements"), _maxStackSize);

    connect(data_system::DataProviderFactory::instance(), SIGNAL(providerSaved(uint)), SLOT(providerSaved(uint)));
    connect(data_system::DataProviderFactory::instance(), SIGNAL(providerDestroyed(uint, uint64_t)), SLOT(providerSavedOrDestroyed(uint, uint64_t)));

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::DisableComponent, this, SLOT(disableController(QVariant)),
                                      qMetaTypeId< QString >(),
                                      QString("visualize_system") );
    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::EnableComponent, this, SLOT(enableController(QVariant)),
                                      qMetaTypeId< QString >(),
                                      QString("visualize_system") );
}

void UndoController::pushUndoAction(QSharedPointer<UndoAction> action)
{
    _stack.push(action);
    if(_stack.size() > _maxStackSize)
    {
        auto it = _stack.begin();
        if(it != _stack.end())
        {
            QSharedPointer<UndoAction> action = *it;
            _stack.erase(it);
            if(action.isNull() == false)
            {
                uint id = action.data()->getId();
                emit undoActionRemoved(id);
                action.clear();
            }
        }
    }
    emit undoControllerChanged();
}

void UndoController::undoAction()
{
    if(_controllerEnabled)
        while(_stack.isEmpty() == false)
        {
            QSharedPointer<UndoAction> action = _stack.pop();
            if(action.isNull() == false)
            {
                uint id = action.data()->getId();
                if(action.data()->undo())
                {
                    emit undoActionRemoved(id);
                    emit undoControllerChanged();
                }
                else
                {
                    _stack.push(action);
                }
                break;
            }
        }
}

void UndoController::removeUndoAction(const QSet<uint> &ids)
{
    bool stackChanged(false);
    QStack< QSharedPointer<UndoAction> > stack;
    foreach(QSharedPointer<UndoAction> action, _stack)
    {
        if(action.isNull() == false)
        {
            uint id = action->getId();
            if(ids.contains(id))
            {
                action.clear();
                stackChanged = true;
                emit undoActionRemoved(id);
            }
            else
            {
                stack.push(action);
            }
        }
        else
        {
            stackChanged = true;
        }
    }

    if(stackChanged)
    {
        _stack = stack;
        emit undoControllerChanged();
    }
}

QList<UndoActionInfo> UndoController::getActionsInfo()
{
    QList<UndoActionInfo> list;
    foreach(QSharedPointer<UndoAction> action, _stack)
    {
        if(action.isNull() == false)
            list.prepend(UndoActionInfo(action.data()->getText(), action.data()->getId()));
    }
    return list;
}

QSharedPointer<UndoAction> UndoController::getActionById(uint id)
{
    QSharedPointer<UndoAction> action;
    foreach(QSharedPointer<UndoAction> _action, _stack)
        if(_action->getId() == id)
        {
            action = _action;
            break;
        }
    return action;
}

void UndoController::providerSaved(uint id)
{
    providerSavedOrDestroyed(id, 0);
}

void UndoController::providerSavedOrDestroyed(uint visualSystemRasterId, uint64_t /*objreprRasterId*/)
{
    bool stackChanged(false);
    QStack< QSharedPointer<UndoAction> > stack;
    foreach(QSharedPointer<UndoAction> action, _stack)
    {
        QSharedPointer<RasterUndoAction> rasterAction = action.dynamicCast<RasterUndoAction>();
        if(rasterAction)
        {
            rasterAction->providerSavedOrDestroyed(visualSystemRasterId);
            if(rasterAction->isValid())
                stack.push(action);
            else
            {
                rasterAction.clear();
                stackChanged = true;
            }
        }
    }

    if(stackChanged)
    {
        _stack = stack;
        emit undoControllerChanged();
    }
}

void UndoController::disableController(QVariant var)
{
    _controllerEnabled = false;
}

void UndoController::enableController(QVariant var)
{
    _controllerEnabled = true;
}





















