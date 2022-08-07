#pragma once
template <typename T> struct LL {
    T data;
    LL<T>* next;
    LL<T>* prev;
};

template <typename T> LL<T>* ListUnlink(LL<T>* data) {
    if(data->prev) data->prev->next = data->next;
    if(data->next) data->next->prev = data->prev;
    return data;
}

template <typename T> void ListLink(LL<T>* prev, LL<T>* next) {
    if(prev) prev->next = next;
    if(next) next->prev = prev;
}