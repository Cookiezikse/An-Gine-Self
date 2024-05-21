#pragma once

#include <vector>
#include <string>

/**
 * @brief The StatusMessage class represents a status message handler.
 */
class StatusMessage
{
public:
    /**
     * @brief Enumeration representing status types.
     */
    enum Status
    {
        Send, /**< Status indicating sending. */
        None /**< Status indicating no operation. */
    };

    /**
     * @brief Gets the unique instance of StatusMessage.
     * @return The unique instance of StatusMessage.
     */
    static StatusMessage& GetInstance() {
        static StatusMessage instance; // Creates a single instance of StatusMessage
        return instance;
    }

    /**
     * @brief Sets the status.
     * @param _newStatus The new status to set.
     */
    void SetStatus(Status _newStatus) {
        status = _newStatus;
    }

    /**
     * @brief Gets the current status.
     * @return The current status.
     */
    Status GetStatus() const {
        return status;
    }

    /**
     * @brief Gets the received messages.
     * @return A constant reference to the received messages.
     */
    const std::vector<std::string>& GetReceivedMessages() const {
        return receivedMessages;
    }

    /**
     * @brief Adds a message to the received messages.
     * @param _message The message to add.
     */
    void AddMessage(const std::string& _message) {
        receivedMessages.push_back(_message);
    }

private:
    /**
     * @brief Constructs a new StatusMessage object.
     */
    StatusMessage() : status(None) {}

    /**
     * @brief Destroys the StatusMessage object.
     */
    ~StatusMessage() {}

    StatusMessage(const StatusMessage&) = delete;
    StatusMessage& operator=(const StatusMessage&) = delete;

    std::vector<std::string> receivedMessages; /**< Vector storing received messages. */
    Status status; /**< Current status. */
};