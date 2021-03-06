//
// PROJECT:         Aspia
// FILE:            host/system_info_request.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "host/system_info_request.h"

namespace aspia {

SystemInfoRequest::SystemInfoRequest(QObject* sender,
                                     proto::system_info::Request&& request,
                                     const char* reply_slot)
    : sender_(sender),
      request_(std::move(request)),
      reply_slot_(reply_slot)
{
    connect(sender, &QObject::destroyed, this, &SystemInfoRequest::senderDestroyed);
}

bool SystemInfoRequest::sendReply(const proto::system_info::Reply& reply)
{
    if (!sender_ || !reply_slot_)
        return false;

    return QMetaObject::invokeMethod(sender_, reply_slot_,
                                     Q_ARG(const proto::system_info::Request&, request_),
                                     Q_ARG(const proto::system_info::Reply&, reply));
}

void SystemInfoRequest::senderDestroyed()
{
    sender_ = nullptr;
    reply_slot_ = nullptr;
}

// static
SystemInfoRequest* SystemInfoRequest::categoryList(QObject* sender, const char* reply_slot)
{
    proto::system_info::Request request;
    request.mutable_category_list_request()->set_dummy(1);
    return new SystemInfoRequest(sender, std::move(request), reply_slot);
}

// static
SystemInfoRequest* SystemInfoRequest::category(QObject* sender,
                                               const QString& uuid,
                                               const QByteArray& params,
                                               const char* reply_slot)
{
    proto::system_info::Request request;
    request.mutable_category_request()->set_uuid(uuid.toStdString());
    request.mutable_category_request()->set_data(params.toStdString());
    return new SystemInfoRequest(sender, std::move(request), reply_slot);
}

} // namespace aspia
