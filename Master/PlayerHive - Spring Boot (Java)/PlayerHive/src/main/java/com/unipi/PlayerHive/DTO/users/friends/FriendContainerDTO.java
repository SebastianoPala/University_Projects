package com.unipi.PlayerHive.DTO.users.friends;

import lombok.AllArgsConstructor;
import lombok.Getter;

import java.util.List;

@Getter
@AllArgsConstructor
public class FriendContainerDTO {

    private List<FriendDTO> friends;

    private int numPages;

    private boolean isLastPage;
}
