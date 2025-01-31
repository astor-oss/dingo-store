/*
 * Copyright 2021 DataCanvas
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package io.dingodb.sdk.common;

public class SDKCommonId implements DingoCommonId {

    private Type type;
    private long parentId;
    private long entityId;

    public SDKCommonId(Type type, long parentId, long entityId) {
        this.type = type;
        this.parentId = parentId;
        this.entityId = entityId;
    }

    @Override
    public Type type() {
        return type;
    }

    @Override
    public long parentId() {
        return parentId;
    }

    @Override
    public long entityId() {
        return entityId;
    }

    @Override
    public String toString() {
        return "CommonId[" + type + "-" + parentId + "-" + entityId + "]";
    }

    @Override
    public boolean equals(Object other) {
        if (this == other) {
            return true;
        }
        if (other == null || getClass() != other.getClass()) {
            return false;
        }

        SDKCommonId that = (SDKCommonId) other;

        if (parentId != that.parentId) {
            return false;
        }
        if (entityId != that.entityId) {
            return false;
        }
        return type == that.type;
    }

    @Override
    public int hashCode() {
        int result = type.hashCode();
        result = 31 * result + (int) (parentId ^ (parentId >>> 32));
        result = 31 * result + (int) (entityId ^ (entityId >>> 32));
        return result;
    }
}
