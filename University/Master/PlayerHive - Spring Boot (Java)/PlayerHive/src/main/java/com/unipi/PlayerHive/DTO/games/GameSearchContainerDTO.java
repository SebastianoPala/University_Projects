package com.unipi.PlayerHive.DTO.games;

import lombok.AllArgsConstructor;
import lombok.Getter;

import java.util.List;

@Getter
@AllArgsConstructor
public class GameSearchContainerDTO {

    private List<GameSearchDTO> searchResult;

    private boolean isLastPage;

}
