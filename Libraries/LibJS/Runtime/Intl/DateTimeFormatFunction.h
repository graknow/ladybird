/*
 * Copyright (c) 2021, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibJS/Forward.h>
#include <LibJS/Runtime/Completion.h>
#include <LibJS/Runtime/NativeFunction.h>

namespace JS::Intl {

class JS_API DateTimeFormatFunction final : public NativeFunction {
    JS_OBJECT(DateTimeFormatFunction, NativeFunction);
    GC_DECLARE_ALLOCATOR(DateTimeFormatFunction);

public:
    static GC::Ref<DateTimeFormatFunction> create(Realm&, DateTimeFormat&);

    virtual ~DateTimeFormatFunction() override = default;
    virtual void initialize(Realm&) override;

    virtual ThrowCompletionOr<Value> call() override;

private:
    explicit DateTimeFormatFunction(DateTimeFormat&, Object& prototype);

    virtual void visit_edges(Visitor&) override;

    GC::Ref<DateTimeFormat> m_date_time_format; // [[DateTimeFormat]]
};

}
